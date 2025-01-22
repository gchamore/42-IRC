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

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}

	int port;
	std::stringstream ss(argv[1]);
	if (!(ss >> port))
	{
		std::cerr << "Error: Invalid port number." << std::endl;
		return 1;
	}

	std::string password = argv[2];

	int server_fd;
	// create bind listen on the server socket
	setup_server(server_fd, port);

	std::vector<pollfd> poll_fds;
	// Add the server socket to the poll vector
	pollfd server_poll_fd = {server_fd, POLLIN, 0};
	poll_fds.push_back(server_poll_fd);

	std::map<int, std::string> client_data;

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
				std::cout << "New client connected: " << client_fd << std::endl;
			}
		}

		for (size_t i = 1; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				int client_fd = poll_fds[i].fd;
				char buffer[1024];
				ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

				if (bytes_read <= 0)
				{
					// Client disconnected are removed from the poll vector and client data map
					std::cout << "Client disconnected: " << client_fd << std::endl;
					close(client_fd);
					poll_fds.erase(poll_fds.begin() + i);
					client_data.erase(client_fd);
					--i; // Adjust index after removal
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

						std::cout << "Received command from client " << client_fd << ": " << command << std::endl;

						// Echo the command back as a simple response
						std::string response = "Echo: " + command + "\n";
						send(client_fd, response.c_str(), response.size(), 0);
					}
				}
			}
		}
	}
	// Close the server socket
	close(server_fd);

	return 0;
}
