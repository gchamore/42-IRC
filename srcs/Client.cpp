/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:21 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/29 13:54:45 by gchamore         ###   ########.fr       */
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
    std::string normalizedData = data;
    
    // Ajouter \r\n à la fin si nécessaire
    if (!normalizedData.empty() && 
        (normalizedData[normalizedData.length() - 1] != '\n' ||
         (normalizedData.length() >= 2 && normalizedData[normalizedData.length() - 2] != '\r')))
    {
        // Si se termine par \n, ajouter \r avant
        if (normalizedData[normalizedData.length() - 1] == '\n')
            normalizedData.insert(normalizedData.length() - 1, "\r");
        else
            normalizedData += "\r\n";
    }

    buffer += normalizedData;
}

bool Client::hasCommand() const
{
    // On ne cherche que \r\n selon la RFC
    return buffer.find("\r\n") != std::string::npos;
}

std::string Client::popCommand()
{
    size_t pos = buffer.find("\r\n");
    if (pos == std::string::npos)
        throw std::logic_error("No complete command available");

    // Inclure le \r\n dans la commande retournée
    std::string command = buffer.substr(0, pos + 2);  // +2 pour inclure \r\n
    buffer.erase(0, pos + 2);
    return command;
}

void Client::sendResponse(const std::string &response)
{
    // Forcer CRLF à la fin
    std::string responseWithCRLF = response;
    if (responseWithCRLF.length() < 2 || 
        responseWithCRLF.substr(responseWithCRLF.length() - 2) != "\r\n")
    {
        responseWithCRLF += "\r\n";
    }

    if (send(fd, responseWithCRLF.c_str(), responseWithCRLF.length(), 0) < 0)
    {
        throw std::runtime_error("Could not send response");
    }
}

std::vector<Channel *> Client::getChannels(const Server &server) const
{
	return server.getChannelsForClient(this);
}
