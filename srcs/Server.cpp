#include "../includes/Server.hpp"

void handleCommand(const CommandParser::ParsedCommand &command, Client &client, const std::string &server_password);

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

	std::cout << "Server setup complete. Listening on port " << port << std::endl;
}

void Server::start()
{
	while (true)
	{
		// Wait for activity on any of the sockets
		int activity = poll(poll_fds.data(), poll_fds.size(), -1);
		if (activity < 0)
		{
			perror("Poll failed");
			break;
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
			"Welcome to the server!\n"
			"Please authenticate using PASS <password>.\n"
			"After authentication, you can use the following commands:\n"
			"- NICK <nickname>: Set your nickname.\n"
			"- USER <username>: Complete your login.\n";
		clients[client_fd]->sendResponse(welcome_message);

		std::cout << "New client connected: " << client_fd << std::endl;
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
					std::cout << "Command from client " << client_fd << ": " << it->command << std::endl;
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
	std::cout << "Client disconnected: " << client_fd << std::endl;
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

void Server::broadcast_message(const std::string &channelName, const std::string &message)
{
	if (channels.find(channelName) != channels.end())
	{
		Channel *channel = channels[channelName];
		const std::vector<Client *> &members = channel->getMembers();
		for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			Client *member = *it;
			member->sendResponse(message);
		}
	}
}

void Server::delete_channel(const std::string& channelName)
{
	if (channels.find(channelName) != channels.end())
	{
		Channel *channel = channels[channelName];
		if (channel->getMembers().empty())
		{
			channels.erase(channelName);
			delete channel;
			std::cout << "Channel " << channelName << " deleted.\n";
		}
	}
}

std::vector<Channel *> Server::getChannelsForClient(const Client* client) const
{
	std::vector<Channel *> clientChannels;
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
