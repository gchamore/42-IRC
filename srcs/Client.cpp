/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:21 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 15:37:18 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <unistd.h>

Client::Client(int socket_fd) : fd(socket_fd), isAuthenticated(false), state(REGISTERING)
{
	if (socket_fd < 0)
	{
		throw std::invalid_argument("Socket file descriptor cannot be negative");
	}
}

int Client::getFD() const
{
	return fd;
}

const std::string &Client::getNickname() const
{
	return nickname;
}

void Client::setNickname(const std::string &nick)
{
	if (nick.empty())
	{
		throw std::invalid_argument("Nickname cannot be empty");
	}
	nickname = nick;
}

const std::string &Client::getUsername() const
{
	return username;
}

void Client::setUsername(const std::string &user)
{
	if (user.empty())
	{
		throw std::invalid_argument("Username cannot be empty");
	}
	username = user;
}

Client::ClientState Client::getState() const
{
	return state;
}

void Client::setState(ClientState newState)
{
	state = newState;
}

bool Client::authenticated() const
{
	return isAuthenticated;
}

void Client::authenticate()
{
	isAuthenticated = true;
}

void Client::appendToBuffer(const std::string &data)
{
	buffer += data;
}

bool Client::hasCommand() const
{
	return buffer.find("\n") != std::string::npos;
}

std::string Client::popCommand()
{
	size_t pos = buffer.find("\n");
	if (pos == std::string::npos)
		throw std::logic_error("No command available");
	std::string command = buffer.substr(0, pos);
	buffer.erase(0, pos + 1);
	return command;
}

void Client::sendResponse(const std::string &response)
{
	std::string responseWithNewline = response + "\r\n";
	if (send(fd, responseWithNewline.c_str(), responseWithNewline.length(), 0) < 0)
	{
		throw std::runtime_error("Could not send response");
	}
}

std::vector<Channel *> Client::getChannels(const Server &server) const
{
	return server.getChannelsForClient(this);
}
