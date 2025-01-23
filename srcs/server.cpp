#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <map>
#include <cstdlib>
#include <sstream>

const std::string AUTH_COMMAND = "PASS";

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

// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}

	int port;
	// Convert the port number from string to integer
	std::stringstream ss(argv[1]);
	if (!(ss >> port))
	{
		std::cerr << "Error: Invalid port number." << std::endl;
		return 1;
	}

	std::string server_password = argv[2];

	int server_fd;
	// create bind listen on the server socket
	setup_server(server_fd, port);

	std::vector<pollfd> poll_fds;
	// Add the server socket to the poll vector
	pollfd server_poll_fd = {server_fd, POLLIN, 0};
	poll_fds.push_back(server_poll_fd);

	// Map to store client data and authentication status username nickname 
	std::map<int, std::string> client_data;
	std::map<int, bool> client_authenticated;
	std::map<int, std::string> client_nicknames;
	std::map<int, std::string> client_usernames;

	while (true)
	{
		// Wait for activity on any of the sockets
		int activity = poll(poll_fds.data(), poll_fds.size(), -1);
		if (activity < 0)
		{
			perror("Poll failed");
			break;
		}

		// Check if the server socket has incoming connections
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

				// Initialize the client's buffer
				client_data[client_fd] = "";
				client_authenticated[client_fd] = false;
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
			if (poll_fds[i].revents & POLLIN)
			{
				// Read data from the client socket
				int client_fd = poll_fds[i].fd;
				char buffer[1024];
				ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
				
				// Handle disconnection
				if (bytes_read == 0)
				{
					// Client disconnected are removed from the poll vector and client data map
					std::cout << "Client disconnected: " << client_fd << std::endl;
					close(client_fd);
					poll_fds.erase(poll_fds.begin() + i);
					client_data.erase(client_fd);
					client_authenticated.erase(client_fd);
					client_nicknames.erase(client_fd);
					--i; // Adjust index after removal
				}
				// Handle receive error
				else if (bytes_read < 0)
				{
					perror("Recv failed");
				}
				else
				{
					// Handle client data
					buffer[bytes_read] = '\0';
					client_data[client_fd] += buffer;

					// Process complete commands ending with '\n'
					size_t pos;
					while ((pos = client_data[client_fd].find('\n')) != std::string::npos)
					{
						std::string command = client_data[client_fd].substr(0, pos);
						client_data[client_fd].erase(0, pos + 1);

						//normalize the command
						std::cout << "Raw command from client " << client_fd << ": " << command << std::endl;
						command = trim(command);
						std::cout << "Normalized command: " << command << std::endl;
						if (command.empty())
						{
							std::cerr << "Error: Empty command received from client " << client_fd << std::endl;
							continue;
						}
						// Handle authentication
						if (!client_authenticated[client_fd])
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
										client_authenticated[client_fd] = true;
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
										client_data.erase(client_fd);
										client_authenticated.erase(client_fd);
										client_nicknames.erase(client_fd);
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
							// Handle NICK/USER commands after authentication
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
							}
						}
					}
				}
			}
		}
	}
	// Close the server socket
	close(server_fd);

	return 0;
}
