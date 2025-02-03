#include "../includes/Server.hpp"

// void handleCommand(const CommandParser::ParsedCommand &command, Client &client, const std::string &server_password);

Server::Server(int port, const std::string &password) : port(port), server_password(password)
{
	setup_server();
}

Server::~Server()
{
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	close(server_fd);
}

void Server::setup_server()
{
	sockaddr_in server_address;

	// create the server socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// set the socket to allow reusing the address and port
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// Set the socket to non-blocking mode
	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("Failed to set non-blocking mode");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// configure the server address struct
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);

	// Bind the socket to the server address
	if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		perror("Bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(server_fd, SOMAXCONN) < 0)
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}

	pollfd server_poll_fd = {server_fd, POLLIN, 0};
	poll_fds.push_back(server_poll_fd);

	std::cout << "Server setup complete.\nWaiting for connections on port " << port << "..." << std::endl;
}

void Server::start()
{
	running = true;

	while (running)
	{
		// Wait for activity on any of the sockets
		int activity = poll(poll_fds.data(), poll_fds.size(), 500);
		if (activity < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			perror("Poll failed");
			break;
		}

		if (activity == 0)
		{
			continue;
		}

		// Handle new connections
		if (poll_fds[0].revents & POLLIN)
		{
			accept_new_client();
		}

		for (size_t i = 1; i < poll_fds.size(); ++i)
		{
			int client_fd = poll_fds[i].fd;

			if (poll_fds[i].revents & POLLIN)
			{
				handle_client_data(client_fd);
			}
		}
	}
}

void Server::stop()
{
	running = false;

	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		it->second->sendResponse(":server.com QUIT :Server shutting down");
	}
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		std::vector<Client *> members = it->second->getMembers();
		for (std::vector<Client *>::iterator it2 = members.begin(); it2 != members.end(); ++it2)
		{
			std::string partMsg = ":" + (*it2)->getNickname() + "!" + (*it2)->getUsername() +
								  "@localhost PART " + it->first + " :Server is shutting down";
			(*it2)->sendResponse(partMsg);
		}
		delete it->second;
	}
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	clients.clear();
	for (size_t i = 0; i < poll_fds.size(); ++i)
	{
		close(poll_fds[i].fd);
	}
	poll_fds.clear();
	close(server_fd);
	std::cout << "Server stopped.\n";
}

void Server::accept_new_client()
{
	sockaddr_in client_address;
	socklen_t client_len = sizeof(client_address);
	int client_fd = accept(server_fd, (sockaddr *)&client_address, &client_len);
	if (client_fd < 0)
	{
		perror("Accept failed");
	}
	else
	{
		// Set the client socket to non-blocking
		fcntl(client_fd, F_SETFL, O_NONBLOCK);

		// Add the new client to the poll vector
		pollfd client_poll_fd = {client_fd, POLLIN, 0};
		poll_fds.push_back(client_poll_fd);

		// Create a new client object and add it to the client data map
		clients[client_fd] = new Client(client_fd);

		// Send a welcome message with command documentation
		std::string welcome_message =
			":server NOTICE Auth :*** Welcome to the IRC Network ***\r\n"
			":server NOTICE Auth :*** Required registration steps:\r\n"
			":server NOTICE Auth :*** 1. PASS <password>\r\n"
			":server NOTICE Auth :*** 2. NICK <nickname> (letters, numbers, and -[]`^{})\r\n"
			":server NOTICE Auth :*** 3. USER <username> <hostname> <servername> :<realname> - All fields required\r\n";
		clients[client_fd]->sendResponse(welcome_message);
	}
}

void Server::handle_client_data(int client_fd)
{
	// Read data from the client socket
	char buffer[1024];
	ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

	// Handle disconnection
	if (bytes_read == 0)
	{
		remove_client(client_fd);
	}
	// Handle receive error
	else if (bytes_read < 0)
	{
		perror("Recv failed");
	}
	else
	{
		// Append the received data to the client buffer
		clients[client_fd]->appendToBuffer(std::string(buffer, bytes_read));
		while (clients.find(client_fd) != clients.end() && clients[client_fd]->hasCommand())
		{
			try
			{
				std::string command = clients[client_fd]->popCommand();
				std::vector<CommandParser::ParsedCommand> parsed_commands = CommandParser::parse(command);

				// Process complete commands ending with '\n'
				for (std::vector<CommandParser::ParsedCommand>::const_iterator it = parsed_commands.begin(); it != parsed_commands.end(); ++it)
				{
					if (DEBUG_MODE)
						std::cout << "Command from client " << clients[client_fd]->getId() << ": " << it->command << std::endl;
					this->handleCommand(*it, *clients[client_fd]);
				}
			}
			catch (const std::exception &e)
			{
				std::cerr << "Error parsing commands: " << e.what() << std::endl;
			}
		}
	}
}

void Server::remove_client(int client_fd)
{
	std::cout << "Client disconnected: " << clients[client_fd]->getNickname() << std::endl;
	std::vector<Channel *> clientChannels = getChannelsForClient(clients[client_fd]);
	for (std::vector<Channel *>::iterator it = clientChannels.begin(); it != clientChannels.end(); ++it)
	{
		(*it)->removeMember(clients[client_fd]);
		if ((*it)->getMembers().empty())
		{
			delete_channel((*it)->getName());
		}
	}
	for (size_t i = 1; i < poll_fds.size(); ++i)
	{
		if (poll_fds[i].fd == client_fd)
		{
			poll_fds.erase(poll_fds.begin() + i);
			break;
		}
	}
	close(client_fd);
	delete clients[client_fd];
	clients.erase(client_fd);
}

void Server::broadcast_message(const std::string &channelName, const std::string &message, Client *exclude)
{
	if (channels.find(channelName) != channels.end())
	{
		Channel *channel = channels[channelName];
		const std::vector<Client *> &members = channel->getMembers();
		for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			Client *member = *it;
			if (member != exclude) // Skip the excluded client
			{
				member->sendResponse(message);
			}
		}
	}
}

void Server::delete_channel(const std::string &channelName)
{
	if (channels.find(channelName) != channels.end())
	{
		Channel *channel = channels[channelName];
		if (channel->getMembers().empty())
		{
			channels.erase(channelName);
			std::cout << "Channel " << channelName << " deleted.\n";
			delete channel;
		}
	}
}

std::vector<Channel *> Server::getChannelsForClient(const Client *client) const
{
	std::vector<Channel *> clientChannels;
	// Parcourt tous les canaux pour trouver ceux où le client est membre
	for (std::map<std::string, Channel *>::const_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		const std::vector<Client *> &members = it->second->getMembers();
		if (std::find(members.begin(), members.end(), client) != members.end())
		{
			clientChannels.push_back(it->second);
		}
	}
	return clientChannels;
}
