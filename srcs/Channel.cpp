/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anferre <anferre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:16 by gchamore          #+#    #+#             */
/*   Updated: 2025/02/03 18:18:46 by anferre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <algorithm>
#include <stdexcept>

Channel::Channel() : creator(NULL)
{
}

Channel::Channel(const std::string &channelName, Client *channelCreator)
	: name(channelName), creator(channelCreator), inviteOnly(false), topic(""), topicRestricted(false), userLimit(-1), password("")
{
	if (!channelCreator)
		throw std::invalid_argument("Channel creator cannot be null");

	if (channelName.length() > Constants::MAX_CHANNEL_NAME_LENGTH)
		throw std::invalid_argument("Channel name exceeds maximum length");

	members.push_back(channelCreator);
	operators[channelCreator] = true; // createur automatiquement operateur
}

void Channel::addMember(Client *client)
{
	if (!client)
		throw std::invalid_argument("Client cannot be null");

	for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if ((*it)->getFD() == client->getFD())
			return;
	}

	members.push_back(client);
	operators[client] = false; // Nouveau membre pas operateur par defaut
}

void Channel::removeMember(Client *client)
{
	if (!client)
		return;

	members.erase(std::remove(members.begin(), members.end(), client), members.end());
	operators.erase(client); // Supprime les privilèges
}

bool Channel::isMember(Client *client) const
{
	if (!client)
		return false;
	return (std::find(members.begin(), members.end(), client) != members.end());
}

bool Channel::isOperator(Client *client) const
{
	if (!client)
		return false;

	std::map<Client *, bool>::const_iterator it = operators.find(client);
	return (it != operators.end() && it->second);
}

bool Channel::isInviteOnly() const
{
	return inviteOnly;
}

void Channel::addInvite(Client *client)
{
	if (!client)
		throw std::invalid_argument("Client cannot be null");
	inviteList.insert(client);
}

void Channel::removeInvite(Client *client)
{
	if (!client)
		throw std::invalid_argument("Client cannot be null");
	inviteList.erase(client);
}

bool Channel::isInvited(Client *client) const
{
	if (!client)
		return false;
	return (inviteList.find(client) != inviteList.end());
}

void Channel::setInviteOnly(bool inviteOnly)
{
	this->inviteOnly = inviteOnly;
}

const std::vector<Client *> &Channel::getMembers() const
{
	return members;
}

const std::string &Channel::getName() const
{
	return name;
}

void Channel::addOperator(Client *client)
{
	if (!client)
		throw std::invalid_argument("Client cannot be null");

	if (!isMember(client))
	{
		client->sendResponse(":server " + ServerMessages::ERR_USERNOTINCHANNEL + " " + client->getNickname() + " " + name + " :They aren't on that channel");
		return;
	}

	if (isOperator(client))
	{
		client->sendResponse(":server " + ServerMessages::ERR_USERONCHANNEL + " " + client->getNickname() + " " + name + " :They are already an operator");
		return;
	}

	operators[client] = true;
}

void Channel::removeOperator(Client *client)
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

void Channel::sendTopic(Client *client) const
{
	if (!client)
		throw std::invalid_argument("Client cannot be null");

	if (!hasTopic())
	{
		client->sendResponse(":server " + ServerMessages::RPL_NOTOPIC + " " + name + " :No topic is set");
	}
	else
	{
		client->sendResponse(":server " + ServerMessages::RPL_TOPIC + " " + name + " :" + topic);
	}
}

void Channel::sendUserList(Client *client) const
{
	if (!client)
		throw std::invalid_argument("Client cannot be null");

	std::string response = ":server " + ServerMessages::RPL_NAMREPLY + " " + client->getNickname() + " = " + name + " :";
	for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		response += (*it)->getNickname() + " ";
	}
	client->sendResponse(response);
}

const std::string &Channel::getTopic() const
{
	return topic;
}

void Channel::setTopic(const std::string &topic)
{
	this->topic = topic;
}

bool Channel::hasTopic() const
{
	return !topic.empty();
}

void Channel::setTopicRestricted(bool restricted)
{
	topicRestricted = restricted;
}

bool Channel::isTopicRestricted() const
{
	return topicRestricted;
}

void Channel::setPassword(const std::string &password)
{
	this->password = password;
}

bool Channel::hasPassword() const
{
	return !password.empty();
}

const std::string &Channel::getPassword() const
{
	return password;
}

void Channel::removePassword()
{
	password.clear();
	this->password = "";
}

void Channel::setUserLimit(int limit)
{
	userLimit = limit;
}

int Channel::getUserLimit() const
{
	return userLimit;
}

bool Channel::isFull() const
{
	return userLimit != -1 && members.size() >= static_cast<size_t>(userLimit);
}
