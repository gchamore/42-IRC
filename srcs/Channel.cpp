/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:16 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 16:19:24 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <algorithm>

Channel::Channel()
{}

Channel::Channel(const std::string& channelName): name(channelName)
{}

void Channel::addMember(Client* client)
{
    if (!client)
    {
        throw std::invalid_argument("Client cannot be null");
    }
    for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if ((*it)->getFD() == client->getFD())
        {
            return;
        }
    }
    members.push_back(client);
}

void Channel::removeMember(Client* client)
{
    if (!client)
    {
        return;
    }
    members.erase(std::remove(members.begin(), members.end(), client), members.end());
}

const std::vector<Client*>& Channel::getMembers() const
{
    return members;
}

const std::string& Channel::getName() const
{
    return name;
}

