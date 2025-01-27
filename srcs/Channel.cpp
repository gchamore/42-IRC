/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:16 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/27 14:31:36 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <algorithm>

Channel::Channel(): creator(NULL)
{}

Channel::Channel(const std::string& channelName, Client* channelCreator)
    : name(channelName), creator(channelCreator)
{
    if (!channelCreator)
        throw std::invalid_argument("Channel creator cannot be null");
    
    members.push_back(channelCreator);
    operators[channelCreator] = true;  // createur automatiquement operateur
}

void Channel::addMember(Client* client)
{
    if (!client)
        throw std::invalid_argument("Client cannot be null");
        
    for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        if ((*it)->getFD() == client->getFD())
            return;
    }
    
    members.push_back(client);
    operators[client] = false;  // Nouveau membre pas operateur par defaut
}

void Channel::removeMember(Client* client)
{
    if (!client)
        return;
        
    members.erase(std::remove(members.begin(), members.end(), client), members.end());
    operators.erase(client); // Supprime les privilèges
}

bool Channel::isOperator(Client* client) const
{
    if (!client)
        return false;
        
    std::map<Client*, bool>::const_iterator it = operators.find(client);
    return (it != operators.end() && it->second);
}

const std::vector<Client*>& Channel::getMembers() const
{
    return members;
}

const std::string& Channel::getName() const
{
    return name;
}

void Channel::addOperator(Client* client)
{
    if (!client)
        throw std::invalid_argument("Client cannot be null");

    // check si membre du chann
    if (std::find(members.begin(), members.end(), client) == members.end())
        throw std::invalid_argument("Client is not a member of this channel");

    // check si le client qui fait la demande est opérateur
    if (!isOperator(creator))
        throw std::runtime_error("Only operators can add new operators");

    operators[client] = true;
}

void Channel::removeOperator(Client* client)
{
    if (!client)
        throw std::invalid_argument("Client cannot be null");

    // On ne peut pas retirer les privilèges du créateur
    if (client == creator)
        throw std::runtime_error("Cannot remove creator's operator privileges");

    // Vérifie si le client est membre du channel
    if (std::find(members.begin(), members.end(), client) == members.end())
        throw std::invalid_argument("Client is not a member of this channel");

    // Vérifie si le client qui fait la demande est opérateur
    if (!isOperator(creator))
        throw std::runtime_error("Only operators can remove operators");

    operators[client] = false;
}

