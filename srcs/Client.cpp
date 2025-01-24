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

Client::Client(int socket_fd): fd(socket_fd), isAuthenticated(false)
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

const std::string& Client::getNickname() const
{
    return nickname;
}

void Client::setNickname(const std::string& nick)
{
    if (nick.empty())
    {
        throw std::invalid_argument("Nickname cannot be empty");
    }
    nickname = nick;
}

const std::string& Client::getUsername() const
{
    return username;
}

void Client::setUsername(const std::string& user)
{
    if (user.empty())
    {
        throw std::invalid_argument("Username cannot be empty");
    }
    username = user;
}

bool Client::authenticated() const
{
    return isAuthenticated;
}

void Client::authenticate()
{
    isAuthenticated = true;
}
