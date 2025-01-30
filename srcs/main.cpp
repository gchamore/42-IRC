/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:23 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/30 16:36:32 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandParser.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include "../includes/Server.hpp"
#include <iostream>
#include <sstream>
#include <map>

bool isValidPort(const std::string &portArg)
{
	std::stringstream ss(portArg);
	int port;
	if (!(ss >> port))
	{
		return false;
	}
	return port > 0 && port <= 65535;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}

	std::string portArg = argv[1];
	std::string server_password = argv[2];

	std::stringstream ss(portArg);
	int port;
	if (!(ss >> port))
	{
		std::cerr << "Error: Invalid port number." << std::endl;
		return 1;
	}

	if (isValidPort(portArg) == false)
	{
		std::cerr << "Invalid port number: " << portArg << std::endl;
		return 1;
	}
	
	std::cout << "\033[1;5;32m\n███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗ \n";
	std::cout << "██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗\n";
	std::cout << "███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝\n";
	std::cout << "╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗\n";
	std::cout << "███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║\n";
	std::cout << "╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝\n\n\033[0m";

	std::cout << "Starting server on port " << port << " with password " << server_password << std::endl;

	Server server(port, server_password);
	server.start();

	return 0;
}

