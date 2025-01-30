/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OperatorCommands.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 13:12:16 by anferre           #+#    #+#             */
/*   Updated: 2025/01/30 16:23:59 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// find a client by nickname
Client *Server::getClientByNickname(const std::string &nickname)
{
	for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

void Server::handleKickCommand(Client &client, const CommandParser::ParsedCommand &command)
{
	if (command.params.size() < 2)
	{
		client.sendResponse("461 KICK :Not enough parameters");
		return;
	}

	std::string channelName = command.params[0];
	std::string nickname = command.params[1];
	std::string reason = (command.params.size() > 2) ? command.params[2] : "Kicked";

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse("403 " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	if (!channel->isOperator(&client))
	{
		client.sendResponse("482 " + channelName + " :You're not a channel operator");
		return;
	}

	// Check if the target exists and is on the channel
	Client *target = this->getClientByNickname(nickname);
	if (!target || !channel->isMember(target))
	{
		client.sendResponse("441 " + nickname + " " + channelName + " :They aren't on that channel");
		return;
	}

	std::string notification = ":" + client.getNickname() + " KICK " + channelName + " " + nickname + " :" + reason;
	this->broadcast_message(channelName, notification);

	channel->removeMember(target);
}

void Server::handleInviteCommand(Client &client, const CommandParser::ParsedCommand &command)
{
	if (command.params.size() < 2)
	{
		client.sendResponse("461 INVITE :Not enough parameters");
		return;
	}

	std::string nickname = command.params[0];
	std::string channelName = command.params[1];

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse("403 " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	// Check if the client is on the channel
	if (!channel->isMember(&client))
	{
		client.sendResponse("442 " + channelName + " :You're not on that channel");
		return;
	}

	// Check if the channel is invite only and the client is not an operator
	if (channel->isInviteOnly() && !channel->isOperator(&client))
	{
		client.sendResponse("482 " + channelName + " :You're not a channel operator");
		return;
	}

	// Check if the target exists
	Client *target = this->getClientByNickname(nickname);
	if (!target)
	{
		client.sendResponse("401 " + nickname + " :No such nick/channel");
		return;
	}

	// If the target is already on the channel, we don't need to send an invite
	if (channel->isMember(target))
	{
		client.sendResponse("443 " + nickname + " " + channelName + " :is already on channel");
		return;
	}

	channel->addInvite(target);
	client.sendResponse("341 " + client.getNickname() + " " + nickname + " " + channelName);
	target->sendResponse(":" + client.getNickname() + " INVITE " + nickname + " :" + channelName);
}

void Server::handleTopicCommand(Client &client, const CommandParser::ParsedCommand &command)
{
	if (command.params.size() < 1)
	{
		client.sendResponse("461 TOPIC :Not enough parameters");
		return;
	}

	std::string channelName = command.params[0];
	std::string topic = (command.params.size() > 1) ? command.params[1] : "";

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse("403 " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	if (command.params.size() == 1)
	{
		if (channel->hasTopic())
			client.sendResponse("332 " + channelName + " :" + channel->getTopic());
		else
			client.sendResponse("331 " + channelName + " :No topic is set");
		return;
	}

	if (channel->isTopicRestricted() && !channel->isOperator(&client))
	{
		client.sendResponse("482 " + channelName + " :You're not a channel operator");
		return;
	}

	std::string newTopic = command.params[1];
	channel->setTopic(newTopic);

	std::string notification = ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic;
	this->broadcast_message(channelName, notification);
}

void Server::handleModeCommand(Client &client, const CommandParser::ParsedCommand &command)
{
    if (command.params.empty())
    {
        client.sendResponse("461 MODE :Not enough parameters");
        return;
    }

    std::string channelName = command.params[0];

    if (command.params.size() == 1)
    {
        if (channels.find(channelName) != channels.end())
        {
            Channel *channel = channels[channelName];
            std::string modes = "+";
            if (channel->isInviteOnly()) modes += "i";
            if (channel->isTopicRestricted()) modes += "t";
            if (channel->hasPassword()) modes += "k";
            if (channel->getUserLimit() > 0) modes += "l";

            client.sendResponse(":server 324 " + client.getNickname() + " " + channelName + " " + modes);
            return;
        }
        else
        {
            client.sendResponse("403 " + channelName + " :No such channel");
            return;
        }
    }
	
    std::string modeChange = command.params[1];

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse("403 " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	if (!channel->isOperator(&client))
	{
		client.sendResponse("482 " + channelName + " :You're not a channel operator");
		return;
	}

	bool adding = true;
	size_t paramIndex = 2;
	Client *target = NULL;

	for (std::string::size_type i = 0; i < modeChange.size(); ++i)
	{
		char mode = modeChange[i];
		if (mode == '+')
		{
			adding = true;
			continue;
		}
		else if (mode == '-')
		{
			adding = false;
			continue;
		}

		switch (mode)
		{
		case 'i':
			channel->setInviteOnly(adding);
			break;
		case 't':
			channel->setTopicRestricted(adding);
			break;
		case 'k':
			if (adding)
			{
				if (command.params.size() <= paramIndex)
				{
					client.sendResponse("461 MODE :Not enough parameters");
					return;
				}
				channel->setPassword(command.params[paramIndex]);
				paramIndex++;
			}
			else
			{
				channel->removePassword();
			}
			break;
		case 'o':
			if (command.params.size() <= paramIndex)
			{
				client.sendResponse("461 MODE :Not enough parameters");
				return;
			}
			target = this->getClientByNickname(command.params[paramIndex]);
			paramIndex++;
			if (!target || !channel->isMember(target))
			{
				client.sendResponse("441 " + target->getNickname() + " " + channelName + " :They aren't on that channel");
				return;
			}
			if (adding)
				channel->addOperator(target);
			else
				channel->removeOperator(target);
			break;
		case 'l':
			if (adding)
			{
				if (command.params.size() <= paramIndex)
				{
					client.sendResponse("461 MODE :Not enough parameters");
					return;
				}
				std::stringstream ss(command.params[paramIndex]);
				int userLimit;
				ss >> userLimit;
				channel->setUserLimit(userLimit);
			}
			else
			{
				channel->setUserLimit(-1);
			}
			break;
		default:
			client.sendResponse("501 MODE :Unknown MODE flag");
			return;
		}
	}
	std::string notification = ":" + client.getNickname() + " MODE " + channelName + " " + modeChange;
	this->broadcast_message(channelName, notification);
}

void Server::handleWhoCommand(const CommandParser::ParsedCommand &command, Client &client)
{
    // Si pas de paramètre, lister tous les utilisateurs visibles
    if (command.params.empty())
    {
        // Parcourir tous les clients connectés
        for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
        {
            Client *target = it->second;
            if (target->getState() == Client::REGISTERED)  // Ne lister que les utilisateurs enregistrés
            {
                // Format: "<client> <user> <host> <server> <nick> <H|G> :<hopcount> <real name>"
                client.sendResponse(":server 352 " + client.getNickname() + " * " +
                                target->getUsername() +     // username
                                " " + "localhost" +         // host
                                " irc.server" +            // server
                                " " + target->getNickname() + // nickname
                                " H" +                      // Here (H) or Gone (G)
                                " :0 " + target->getUsername()); // hopcount et realname
            }
        }
        client.sendResponse(":server 315 " + client.getNickname() + " * :End of WHO list");
        return;
    }
    else
    {
        std::string target = command.params[0];
        if (channels.find(target) != channels.end())
        {
            Channel *channel = channels[target];
            const std::vector<Client *> &members = channel->getMembers();
            
            // Envoyer d'abord les informations du canal
            for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                std::string prefix = channel->isOperator(*it) ? "@" : "";
                
                // Format : "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
                client.sendResponse(":server 352 " + client.getNickname() + " " + target + 
                                 " " + (*it)->getUsername() +     // username
                                 " localhost" +                   // host
                                 " irc.server" +                  // server
                                 " " + (*it)->getNickname() +     // nickname
                                 " H" + prefix +                  // Here + operator status
                                 " :0 " + (*it)->getUsername());  // hopcount + realname
            }

            // Message de fin
            client.sendResponse(":server 315 " + client.getNickname() + " " + target + " :End of WHO list");
        }
        else
        {
            client.sendResponse(":server 403 " + client.getNickname() + " " + target + " :No such channel");
        }
    }
}