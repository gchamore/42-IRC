#include "../includes/Server.hpp"

const std::string AUTH_COMMAND = "PASS";
void handleCommand(const CommandParser::ParsedCommand &command, Client &client, const std::string &server_password);

void setup_server(int &server_fd, int port)
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

	std::cout << "Server setup complete. Listening on port " << port << std::endl;
}

void server(int server_fd, std::vector<pollfd> poll_fds, std::string server_password)
{
	std::map<int, Client*> clients;

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
				clients[client_fd] = new Client(client_fd);

				// Send a welcome message with command documentation
				std::string welcome_message =
					"Welcome to the server!\n"
					"Please authenticate using PASS <password>.\n"
					"After authentication, you can use the following commands:\n"
					"- NICK <nickname>: Set your nickname.\n"
					"- USER <username>: Complete your login.\n";
				send(client_fd, welcome_message.c_str(), welcome_message.size(), 0);

				std::cout << "New client connected: " << client_fd << std::endl;
			}
		}

		for (size_t i = 1; i < poll_fds.size(); ++i)
		{
			int client_fd = poll_fds[i].fd;

			if (poll_fds[i].revents & POLLIN)
			{
				// Read data from the client socket
				char buffer[1024];
				ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

				// Handle disconnection
				if (bytes_read == 0)
				{
					// Client disconnected are removed from the poll vector and client data map
					std::cout << "Client disconnected: " << client_fd << std::endl;
					close(client_fd);
					poll_fds.erase(poll_fds.begin() + i);
					clients.erase(client_fd);
					--i; // Adjust index after removal
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
					while (clients[client_fd]->hasCommand())
					{
						try
						{
							std::string command = clients[client_fd]->popCommand();
							std::vector<CommandParser::ParsedCommand> parsed_commands = CommandParser::parse(command);

							// Process complete commands ending with '\n'
							for (std::vector<CommandParser::ParsedCommand>::const_iterator it = parsed_commands.begin(); it != parsed_commands.end(); ++it)
							{
								handleCommand(*it, *clients[client_fd], server_password);
							}
						}
						catch (const std::exception &e)
						{
							std::cerr << "Error parsing commands: " << e.what() << std::endl;
						}
					}
				}
			}
		}
	}
	for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	// Close the server socket
	close(server_fd);
}

/*// Handle NICK/USER commands after authentication
							if (command.find("NICK") == 0)
							{
								//check for missing nickname
								if (command.size() <= 5)
								{
									std::string response = "Error: Please provide a nickname. Use NICK <nickname>\n";
									send(client_fd, response.c_str(), response.size(), 0);
									std::cerr << "Error: Client " << client_fd << " sent NICK without an argument.\n";
								}
								else
								{
									std::string nickname = command.substr(5);
									// check for invalid nickname
									if (nickname.empty() || nickname.find(' ') != std::string::npos)
									{
										std::string response = "Error: Invalid nickname format. Use NICK <nickname>\n";
										send(client_fd, response.c_str(), response.size(), 0);
									}
									else
									{
										client_nicknames[client_fd] = nickname;
										std::string response = "Nickname set to: " + nickname + "\n";
										send(client_fd, response.c_str(), response.size(), 0);
									}
								}
							}
							// Handle USER command
							else if (command.find("USER") == 0)
							{
								//check for missing username
								if (command.size() <= 5)
								{
									std::string response = "Error: Please provide a username. Use USER <username>\n";
									send(client_fd, response.c_str(), response.size(), 0);
									std::cerr << "Error: Client " << client_fd << " sent USER without an argument.\n";
								}
								else
								{
									//check if the nickname is set before the username
									if (client_nicknames[client_fd].empty())
									{
										std::string response = "Error: Please set a nickname using NICK <nickname> before USER.\n";
										send(client_fd, response.c_str(), response.size(), 0);
									}
									else
									{
										std::string username = command.substr(5);
										//check for invalid username
										if (username.empty() || username.find(' ') != std::string::npos)
										{
											std::string response = "Error: Invalid username format. Use USER <username>\n";
											send(client_fd, response.c_str(), response.size(), 0);
										}
										else
										{
											client_usernames[client_fd] = username;

											// Check if login is complete
											if (!client_usernames[client_fd].empty() && !client_nicknames[client_fd].empty())
											{
												std::string response = "Welcome, " + client_nicknames[client_fd] + "!\n";
												send(client_fd, response.c_str(), response.size(), 0);
												std::cout << "Client " << client_fd << " has completed login with username: " << username << std::endl;
											}
										}
									}
								}
								}
							else
							{
								std::string response = "Unknown command\n";
								send(client_fd, response.c_str(), response.size(), 0);
							}*/

/*std::string command = clients[client_fd].popCommand();
						std::cout << "Raw command from client " << client_fd << ": " << command << std::endl;
						if (command.empty())
						{
							std::cerr << "Error: Empty command received from client " << client_fd << std::endl;
							continue;
						}
						// Handle authentication
						if (!clients[client_fd].authenticated())
						{
							// Check if the command is an authentication command
							if (command.find(AUTH_COMMAND) == 0)
							{
								// Check if the password is provided
								if (command.size() > AUTH_COMMAND.size() + 1)
								{
									std::string received_password = command.substr(AUTH_COMMAND.size() + 1);
									// Check if the password is correct
									if (received_password == server_password)
									{
										clients[client_fd].authenticate();
										std::string response = "Authentication successful\n";
										send(client_fd, response.c_str(), response.size(), 0);
										std::cout << "Client " << client_fd << " authenticated." << std::endl;
									}
									else
									{
										std::string response = "Authentication failed\n";
										send(client_fd, response.c_str(), response.size(), 0);
										close(client_fd);
										poll_fds.erase(poll_fds.begin() + i);
										clients.erase(client_fd);
										--i;
									}
								}
								else
								{
									std::string response = "Please provide a password.   Use PASS <password>\n";
									send(client_fd, response.c_str(), response.size(), 0);
								}
							}
							else
							{
								std::string response = "Please authenticate using PASS <password>\n";
								send(client_fd, response.c_str(), response.size(), 0);
							}
						}
						else
						{
						}*/