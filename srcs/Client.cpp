/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:21 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/30 15:12:03 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <unistd.h>

// Initialiser le compteur statique
size_t Client::userCount = 0;

Client::Client(int socket_fd) : fd(socket_fd), isAuthenticated(false), state(REGISTERING)
{
    if (socket_fd < 0)
    {
        throw std::invalid_argument("Socket file descriptor cannot be negative");
    }
    userId = ++userCount;  // Assigner un ID séquentiel commençant à 1
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
    if (nick.empty() || nick == " ")
    {
        throw std::invalid_argument("Nickname cannot be empty or just whitespace");
    }
    if (nick.length() > 9)
    {
        throw std::invalid_argument("Nickname cannot be longer than 9 characters");
    }
    
    // Vérification supplémentaire des caractères valides
    const std::string specialChars = "-[]`^{}";
    if (!std::isalpha(nick[0]))
    {
        throw std::invalid_argument("Nickname must start with a letter");
    }
    
    for (size_t i = 1; i < nick.length(); ++i)
    {
        char c = nick[i];
        if (!std::isalnum(c) && specialChars.find(c) == std::string::npos)
        {
            throw std::invalid_argument("Invalid character in nickname");
        }
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
    // Debug simple du buffer
    // std::cout << "Current buffer before append: '" << buffer << "'" << std::endl;
    // std::cout << "New data to append: '" << data << "'" << std::endl;

    buffer += data;

    // Nettoyage des doubles CRLF potentiels
    size_t pos;
    while ((pos = buffer.find("\r\n\r\n")) != std::string::npos)
    {
        buffer.erase(pos, 2);  // Supprime un des deux \r\n
    }
}

bool Client::hasCommand() const
{
    return buffer.find("\r\n") != std::string::npos;
}

std::string Client::popCommand()
{
    size_t pos = buffer.find("\r\n");
    if (pos == std::string::npos)
        throw std::logic_error("No complete command available");

    std::string command = buffer.substr(0, pos + 2);
    buffer.erase(0, pos + 2);

    // Debug simple de la commande
    // std::cout << "Popped command: '" << command.substr(0, command.length() - 2) << "'" << std::endl;

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
