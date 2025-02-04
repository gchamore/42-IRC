/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OperatorCommands.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anferre <anferre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 13:12:16 by anferre           #+#    #+#             */
/*   Updated: 2025/02/04 12:29:24 by anferre          ###   ########.fr       */
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
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " KICK :Not enough parameters");
		return;
	}

	std::string channelName = command.params[0];
	std::string nickname = command.params[1];
	std::string reason = (command.params.size() > 2) ? command.params[2] : "Kicked";
	std::cout << "KICK command received: " << command.params[1] << "leaved" << command.params[0] << std::endl;

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	if (!channel->isOperator(&client))
	{
		client.sendResponse(":server " + ServerMessages::ERR_CHANOPRIVSNEEDED + " " + channelName + " :You're not a channel operator");
		return;
	}

	// Check if the target exists and is on the channel
	Client *target = this->getClientByNickname(nickname);
	if (!target || !channel->isMember(target))
	{
		client.sendResponse(":server " + ServerMessages::ERR_USERNOTINCHANNEL + " " + nickname + " " + channelName + " :They aren't on that channel");
		return;
	}

	std::string notification = ":" + client.getNickname() + "!" + client.getUsername() + "@" + "localhost" + " KICK " + channelName + " " + nickname + " :" + reason;
	this->broadcast_message(channelName, notification, NULL);
	channel->removeMember(target);
	if (channel->getMembers().empty())
	{
		this->delete_channel(channelName);
	}
}

void Server::handleInviteCommand(Client &client, const CommandParser::ParsedCommand &command)
{
	if (command.params.size() < 2)
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " INVITE :Not enough parameters");
		return;
	}

	std::string nickname = command.params[0];
	std::string channelName = command.params[1];
	std::cout << "INVITE command received: " << command.params[0] << " invited on " << command.params[1] << std::endl;

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	// Check if the client is on the channel
	if (!channel->isMember(&client))
	{
		client.sendResponse(":server " + ServerMessages::ERR_NOTONCHANNEL + " " + channelName + " :You're not on that channel");
		return;
	}

	// Check if the channel is invite only and the client is not an operator
	if (channel->isInviteOnly() && !channel->isOperator(&client))
	{
		client.sendResponse(":server " + ServerMessages::ERR_CHANOPRIVSNEEDED + " " + channelName + " :You're not a channel operator");
		return;
	}

	// Check if the target exists
	Client *target = this->getClientByNickname(nickname);
	if (!target)
	{
		client.sendResponse(":server " + ServerMessages::ERR_NOSUCHNICK + " " + nickname + " :No such nick/channel");
		return;
	}

	// If the target is already on the channel, we don't need to send an invite
	if (channel->isMember(target))
	{
		client.sendResponse(":server " + ServerMessages::ERR_USERONCHANNEL + " " + nickname + " " + channelName + " :is already on channel");
		return;
	}

	channel->addInvite(target);
	client.sendResponse(":server " + ServerMessages::RPL_INVITING + " " + client.getNickname() + " " + nickname + " " + channelName);
	target->sendResponse(":" + client.getNickname() + " INVITE " + nickname + " :" + channelName);
}

void Server::handleTopicCommand(Client &client, const CommandParser::ParsedCommand &command)
{
	if (command.params.size() < 1)
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " TOPIC :Not enough parameters");
		return;
	}

	std::string channelName = command.params[0];
	std::string topic = (command.params.size() > 1) ? command.params[1] : "";
	std::cout << "TOPIC command received: " << "on channel :" << command.params[0] << std::endl;

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	if (command.params.size() == 1)
	{
		if (channel->hasTopic())
			client.sendResponse(":server " + ServerMessages::RPL_TOPIC + " " + channelName + " :" + channel->getTopic());
		else
			client.sendResponse(":server " + ServerMessages::RPL_NOTOPIC + " " + channelName + " :No topic is set");
		return;
	}

	if (channel->isTopicRestricted() && !channel->isOperator(&client))
	{
		client.sendResponse(":server " + ServerMessages::ERR_CHANOPRIVSNEEDED + " " + channelName + " :You're not a channel operator");
		return;
	}

	std::string newTopic = command.params[1];
	channel->setTopic(newTopic);

	std::string notification = ":" + client.getNickname() + " TOPIC " + channelName + " :" + newTopic;
	this->broadcast_message(channelName, notification, NULL);
}

static void handleInviteMode(bool adding, Channel *channel)
{
	channel->setInviteOnly(adding);
}

static void handleTopic(bool adding, Channel *channel)
{
	channel->setTopicRestricted(adding);
}

static void handlePasswordMode(bool adding, const CommandParser::ParsedCommand &command, size_t &paramIndex, Client &client, Channel *channel)
{
	if (adding)
	{
		if (command.params.size() <= paramIndex)
		{
			client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " MODE :Not enough parameters");
			return;
		}
		std::string password = command.params[paramIndex++];
		std::cout << "password : $" << password << "$"<< std::endl;
		bool isWhitespaceOnly = true;
		for (size_t i = 0; i < password.size(); ++i)
		{
			if (!std::isspace(static_cast<unsigned char>(password[i])))
			{
				isWhitespaceOnly = false;
				break;
			}
		}

		if (isWhitespaceOnly)
		{
			client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " MODE :Not enough parameters");
			return;
		}
		channel->removePassword();
		channel->setPassword(password);
	}
	else
	{
		channel->removePassword();
		
	}
}

static void handleOperatorMode(bool adding, const CommandParser::ParsedCommand &command, size_t &paramIndex, Client &client, Channel *channel, const std::string &channelName, Server &server)
{
	if (command.params.size() <= paramIndex)
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " MODE :Not enough parameters");
		return;
	}

	Client *target = server.getClientByNickname(command.params[paramIndex++]);
	if (!target || !channel->isMember(target))
	{
		client.sendResponse(":server " + ServerMessages::ERR_USERNOTINCHANNEL + " " + target->getNickname() + " " + channelName + " :They aren't on that channel");
		return;
	}

	if (adding)
		channel->addOperator(target);
	else
		channel->removeOperator(target);
}

static void handleUserLimitMode(bool adding, const CommandParser::ParsedCommand &command, size_t &paramIndex, Client &client, Channel *channel)
{
	if (adding)
	{
		if (command.params.size() <= paramIndex)
		{
			client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " MODE :Not enough parameters");
			return;
		}
		std::stringstream ss(command.params[paramIndex++]);
		int userLimit;
		ss >> userLimit;
		channel->setUserLimit(userLimit);
	}
	else
	{
		channel->setUserLimit(-1);
	}
}

void Server::handleModeCommand(Client &client, const CommandParser::ParsedCommand &command)
{
	if (command.params.empty())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " " + client.getNickname() + " MODE :Not enough parameters");
		return;
	}
	std::string channelName = command.params[0];

	if (channels.find(channelName) == channels.end())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + channelName + " :No such channel");
		return;
	}

	Channel *channel = channels[channelName];

	if (command.params.size() == 1)
	{
		std::string modes = "+n"; // Default modes: No external messages & topic restricted
		if (channel->isInviteOnly())
			modes += "i";
		if (channel->hasPassword())
			modes += "k";
		if (channel->getUserLimit() > 0)
			modes += "l";

		client.sendResponse(":server " + ServerMessages::RPL_CHANNELMODEIS + " " + client.getNickname() + " " + channelName + " " + modes);
		return;
	}

	std::string modeChange = command.params[1];
	std::cout << "MODE command received: " << command.params[0] << " " << command.params[1] << std::endl;
	std::set<char> activeModes;

	// Certains modes sont incompatibles entre eux
	if (activeModes.find('i') != activeModes.end() &&
		activeModes.find('p') != activeModes.end())
	{
		client.sendResponse(":server " + ServerMessages::ERR_UNKNOWNMODE + " :is unknown mode to me for " + channelName);
		return;
	}

	if (!channel->isOperator(&client))
	{
		client.sendResponse(":server " + ServerMessages::ERR_CHANOPRIVSNEEDED + " " + channelName + " :You're not a channel operator");
		return;
	}

	bool adding = true;
	size_t paramIndex = 2;

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
			handleInviteMode(adding, channel);
			break;
		case 't':
			handleTopic(adding, channel);
			break;
		case 'k':
			handlePasswordMode(adding, command, paramIndex, client, channel);
			break;
		case 'o':
			handleOperatorMode(adding, command, paramIndex, client, channel, channelName, *this);
			break;
		case 'l':
			handleUserLimitMode(adding, command, paramIndex, client, channel);
			break;
		default:
			client.sendResponse(":server " + ServerMessages::ERR_UMODEUNKNOWNFLAG + " MODE :Unknown MODE flag");
		}
		std::string notification = ":" + client.getNickname() + " MODE " + channelName + " " + modeChange;
		this->broadcast_message(channelName, notification, NULL);
		client.sendResponse(":server " + ServerMessages::RPL_CHANNELMODEIS + client.getNickname() + " " + channelName + " " + modeChange);
	}
}
