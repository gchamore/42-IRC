/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCommands.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anferre <anferre@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 15:32:35 by anferre           #+#    #+#             */
/*   Updated: 2025/02/04 14:43:43 by anferre          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

static std::vector<std::string> split(const std::string &input, char delimiter)
{
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		result.push_back(token);
	}

	return result;
}

void Server::handleCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	// CAP & PASS command must be first for non-authenticated clients
	if (!client.authenticated())
	{
		if (command.command == "CAP")
		{
			if (DEBUG_MODE)
				std::cout << "CAP command received" << std::endl;
			client.sendResponse("CAP * LS :multi-prefix sasl"); // Standard CAP LS response
			if (DEBUG_MODE)
				std::cout << "CAP LS sent" << std::endl;
			return;
		}
		if (command.command == "PASS")
		{
			if (command.params.empty())
            {
				client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " PASS :Not enough parameters");
				return;
			}
			if (DEBUG_MODE)
				std::cout << "is Authenticated: " << client.authenticated() << std::endl;
			handlePassCommand(command, client);
			std::cout << "User number : " << client.getId() << "\n" << std::endl;
			std::cout << "PASS command received: " << command.params[0] << std::endl;
			if (DEBUG_MODE)
				std::cout << "is Authenticated: " << client.authenticated() << std::endl;
		}
		return; // Silently ignore other commands until authenticated
	}
	

	// Handle registration state commands
	if (client.getState() == Client::REGISTERING)
	{
		if (command.command == "NICK")
		{
			if (DEBUG_MODE)
				std::cout << "Nick before: " << client.getNickname() << std::endl;
			handleNickCommand(command, client);
			std::cout << "NICK command received:" << client.getNickname() << std::endl;
			if (DEBUG_MODE)
				std::cout << "Nick after: " << client.getNickname() << std::endl;
		}
		else if (command.command == "USER")
		{
			if (DEBUG_MODE)
				std::cout << "User before: " << client.getUsername() << std::endl;
			handleUserCommand(command, client);
			if (!command.params.empty())
				std::cout << "User command received:" << command.params[0] << std::endl;
			if (DEBUG_MODE)
				std::cout << "User after: " << client.getUsername() << std::endl;
		}
		else
			client.sendResponse(":server " + ServerMessages::ERR_NOTREGISTERED + " :You have not registered");
	}
	else if (client.getState() == Client::REGISTERED)
	{
		if (command.command == "NICK")
			handleNickCommand(command, client);
		else if (command.command == "JOIN")
			handleJoinCommand(command, client);
		else if (command.command == "WHO")
			handleWhoCommand(command, client);
		else if (command.command == "PRIVMSG")
			handlePrivmsgCommand(command, client);
		else if (command.command == "PART")
			handlePartCommand(command, client);
		else if (command.command == "QUIT")
			handleQuitCommand(command, client);
		else if (command.command == "MODE")
			handleModeCommand(client, command);
		else if (command.command == "KICK")
			handleKickCommand(client, command);
		else if (command.command == "INVITE")
			handleInviteCommand(client, command);
		else if (command.command == "TOPIC")
			handleTopicCommand(client, command);
		else if (command.command == "PASS")
			client.sendResponse(":server " + ServerMessages::ERR_ALREADYREGISTERED + " * :You may not reregister");
		else if (command.command == "USER")
			client.sendResponse(":server " + ServerMessages::ERR_ALREADYREGISTERED + " * :You may not reregister");
		else if (command.command == "CAP")
			client.sendResponse(":server " + ServerMessages::ERR_ALREADYREGISTERED + " * :You may not reregister");
		else
			client.sendResponse(":server " + ServerMessages::ERR_UNKNOWNCOMMAND + " * " + command.command + " :Unknown command");
	}
}

void Server::handlePassCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (client.authenticated())
	{
		client.sendResponse(":server " + ServerMessages::ERR_ALREADYREGISTERED + " * :You may not reregister");
		return;
	}

	if (command.params.empty())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " * PASS :Not enough parameters");
		return;
	}

	// Trim any whitespace from the password
	std::string provided_password = command.params[0];
	provided_password.erase(0, provided_password.find_first_not_of(" \t\r\n"));
	provided_password.erase(provided_password.find_last_not_of(" \t\r\n") + 1);

	if (provided_password != server_password)
	{
		client.sendResponse(":server " + ServerMessages::ERR_PASSWDMISMATCH + " * :Password incorrect");
		return;
	}
	client.authenticate();
	client.sendResponse(":server NOTICE Auth :Password accepted");
}

bool Server::isValidNickname(const std::string &nickname)
{
	const std::string specialChars = "-[]`^{}";

	if (nickname.empty() || nickname.length() > Constants::MAX_NICKNAME_LENGTH)
        return false;

	if (!std::isalpha(nickname[0]) && nickname[0] != '[' && 
        nickname[0] != ']' && nickname[0] != '\\' && 
        nickname[0] != '`' && nickname[0] != '_' && 
        nickname[0] != '^' && nickname[0] != '{' && 
        nickname[0] != '|' && nickname[0] != '}')
        return false;

	for (size_t i = 1; i < nickname.length(); i++)
	{
        char c = nickname[i];
        if (!std::isalnum(c) && c != '-' && c != '[' && 
            c != ']' && c != '\\' && c != '`' && 
            c != '_' && c != '^' && c != '{' && 
            c != '|' && c != '}')
            return false;
	}
	return true;
}

bool Server::isNicknameTaken(const std::string &nickname) const
{
	for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
		{
			return true;
		}
	}
	return false;
}

void Server::handleNickCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	try
	{
		if (command.params.empty() || command.params[0].empty())
		{
			client.sendResponse(":server " + ServerMessages::ERR_NONICKNAMEGIVEN + " * :No nickname given - Usage: NICK <nickname>");
			return;
		}

		std::string nick = command.params[0];

		if (!this->isValidNickname(nick))
		{
			client.sendResponse(":server " + ServerMessages::ERR_ERRONEUSNICKNAME + " * " + nick + " :Erroneous nickname");
			return;
		}

		// Si le nickname est déjà pris, ajouter un suffixe numérique
		std::string originalNick = nick;
		int suffix = 1;
		while (this->isNicknameTaken(nick))
		{
			// Si c'est le même client qui essaie de changer son propre nickname
			if (!client.getNickname().empty() && client.getNickname() == nick)
				break;

			std::stringstream ss;
			ss << originalNick << suffix++;
			nick = ss.str();

			// Vérifier que le nouveau nickname avec suffixe est toujours valide
			if (nick.length() > 9)
			{
				client.sendResponse(":server " + ServerMessages::ERR_NICKNAMEINUSE + " * " + originalNick +
									" :Nickname is already in use and alternative too long. Please choose another");
				return;
			}
			if (!client.getNickname().empty())
			{
				std::string notification = ":" + client.getNickname() + " changed NICK for :" + command.params[0];
			}
		}
		std::string oldNick = client.getNickname().empty() ? "*" : client.getNickname();
		client.setNickname(nick);

		if (oldNick == "*")
		{
			client.sendResponse(":server NOTICE Auth :Nickname successfully set to " + nick +
								(nick != originalNick ? " (original choice was taken)" : ""));
		}
		else
		{
			client.sendResponse(":" + oldNick + " NICK :" + nick);
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error in handleNickCommand: " << e.what() << std::endl;
		client.sendResponse(":server 432 * :Error setting nickname: " + std::string(e.what()));
	}
}

bool Server::isValidUsername(const std::string &username)
{
	// Check length (1–10 characters)
	if (username.empty() || username.length() > Constants::MAX_USERNAME_LENGTH)
        return false;

	// Check allowed characters
	for (size_t i = 0; username[i]; ++i)
	{
        char c = username[i];
        // Check for forbidden characters
        if (c == ' ' || c == '@' || c == '\0' || c == '\r' || c == '\n')
            return false;
        // Check for allowed characters
        if (!std::isalnum(c) && c != '-' && c != '.' && c != '_' && c != '~')
            return false;
    }

	return true;
}
void Server::handleUserCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (client.getState() != Client::REGISTERING)
	{
		client.sendResponse(":server " + ServerMessages::ERR_ALREADYREGISTERED + " * :You may not reregister");
		return;
	}
	if (!client.authenticated())
	{
		client.sendResponse(":server " + ServerMessages::ERR_PASSWDMISMATCH + " * :You must send PASS first");
		return;
	}
	if (client.getNickname().empty())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NONICKNAMEGIVEN + " * :No nickname given");
		return;
	}
	if (command.params.size() < 1)
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " * USER :Not enough parameters. Usage: USER <username> <hostname> <servername> :<realname>");
		return;
	}
	if (!this->isValidUsername(command.params[0]))
	{
		client.sendResponse(":server " + ServerMessages::ERR_ERRONEUSNICKNAME + " * :Erroneous username");
		return;
	}
	client.setUsername(command.params[0]);
	client.setState(Client::REGISTERED);

	// Envoyer une confirmation de l'username
	client.sendResponse(":server NOTICE Auth :Username successfully set to " + command.params[0]);

	// Puis envoyer le message de bienvenue
	client.sendResponse(":server 001 " + client.getNickname() +
						" :Welcome to the IRC Network, " + client.getNickname() + "!");
}

bool Server::isValidChannelName(const std::string &channelName) const
{
	// Le nom doit commencer par '#'
	if (channelName.empty())
		return false;

	if (channelName[0] != '#')
	{
		std::cout << "Channel name must start with '#'" << std::endl;
		return false;
	}

	// Vérifier la longueur
	if (channelName.length() < 2)
	{
		std::cout << "Channel name too short" << std::endl;
		return false;
	}

	if (channelName.length() > Constants::MAX_CHANNEL_NAME_LENGTH)
	{
		std::cout << "Channel name too long (max " << Constants::MAX_CHANNEL_NAME_LENGTH << " characters)" << std::endl;
		return false;
	}

	// Vérifier les caractères invalides
	const std::string invalidChars = " ,\a\r\n";
	if (channelName.find_first_of(invalidChars) != std::string::npos)
	{
		std::cout << "Channel name contains invalid characters" << std::endl;
		return false;
	}

	return true;
}

void Server::handleJoinCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	// 1. Validation des paramètres
	if (command.params.empty())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " " + client.getNickname() + " JOIN :Not enough parameters");
		return;
	}
	std::cout << "JOIN command received: " << command.params[0] << std::endl;


	// 2. Parse les canaux et mots de passe
	std::vector<std::string> channelNames = split(command.params[0], ',');
	std::vector<std::string> passwords;
	if (command.params.size() > 1)
	{
		passwords = split(command.params[1], ',');
	}
	
	// Vérifier la limite maximale de canaux par client (typiquement 10)
	if (client.getChannels(*this).size() + channelNames.size() >= 10)
	{
		client.sendResponse(":server " + ServerMessages::ERR_TOOMANYCHANNELS + " * :You have joined, or trying to join too many channels");
		return;
	}

	if (!passwords.empty() && channelNames.size() != passwords.size())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " " + client.getNickname() + " JOIN :Mismatched number of channels and passwords");
		return;
	}

	// 3. Traitement pour chaque canal
	for (size_t i = 0; i < channelNames.size(); ++i)
	{
		std::string channelName = channelNames[i];
		std::string providedPassword = (i < passwords.size()) ? passwords[i] : "";

		// Vérification du nom du canal
		if (!isValidChannelName(channelName))
		{
			if (channelName[0] != '#')
			{
				client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + channelName +
									" :No such channel - Channel name must start with '#'");
			}
			else
			{
				client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + channelName +
									" :Invalid channel name");
			}
			continue;
		}

		// 4. Création ou accès au canal
		bool isNewChannel = false;
		if (channels.find(channelName) == channels.end())
		{
			channels[channelName] = new Channel(channelName, &client);
			isNewChannel = true;
			std::cout << "Channel created: " << channelName << std::endl;
			if (!providedPassword.empty())
			{
				channels[channelName]->setPassword(providedPassword);
			}
		}

		Channel *channel = channels[channelName];

		// 5. Vérification des restrictions
		if (!isNewChannel)
		{
			if (channel->isFull())
			{
				client.sendResponse(":server " + ServerMessages::ERR_CHANNELISFULL + " " + client.getNickname() + " " + channelName + " :Cannot join channel (+l) limit set, channel is full");
				continue;
			}
			if (channel->hasPassword() && channel->getPassword() != providedPassword)
			{
				client.sendResponse(":server " + ServerMessages::ERR_BADCHANNELKEY + " " + client.getNickname() + " " + channelName + " :Cannot join channel (+k) wrong key");
				continue;
			}
			if (channel->isInviteOnly() && !channel->isInvited(&client))
			{
				client.sendResponse(":server " + ServerMessages::ERR_INVITEONLYCHAN + " " + client.getNickname() + " " + channelName + " :Cannot join channel (+i) not invited");
				continue;
			}
			if (channel->getMembers().size() >= Constants::MAX_USERS_PER_CHANNEL)
			{
				client.sendResponse(":server " + ServerMessages::ERR_CHANNELISFULL + " " + client.getNickname() + " " + channelName + " :Channel is full");
				continue;
			}
		}

		// 6. Ajout du membre
		channel->addMember(&client);
		channel->removeInvite(&client);

		// 7. Notifications
		std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@server JOIN " + channelName;
		broadcast_message(channelName, joinMsg, NULL);

		// Topic
		client.sendResponse(":server " + ServerMessages::RPL_TOPIC + " " + client.getNickname() + " " + channelName + " :Welcome to " + channelName);

		// Liste des membres
		std::string memberList = ":server 353 " + client.getNickname() + " = " + channelName + " :";
		const std::vector<Client *> &members = channel->getMembers();
		for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			if (channel->isOperator(*it))
				memberList += "@";
			memberList += (*it)->getNickname() + " ";
		}
		client.sendResponse(":server " + ServerMessages::RPL_NAMREPLY + " " + client.getNickname() + " = " + channelName + " :" + memberList);
		client.sendResponse(":server " + ServerMessages::RPL_ENDOFNAMES + " " + client.getNickname() + " " + channelName + " :End of /NAMES list.");

		// 8. Logging
		std::cout << "\nChannel " << channelName << " updated:" << std::endl;
		std::cout << "Members (" << members.size() << "): ";
		for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			std::cout << (channel->isOperator(*it) ? "@" : "") << (*it)->getNickname() << " ";
		}
		std::cout << std::endl;
	}
}

void Server::handlePrivmsgCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (command.params.size() < 2)
	{
		client.sendResponse(":server " + ServerMessages::ERR_NEEDMOREPARAMS + " * PRIVMSG :Not enough parameters");
		return;
	}
	std::cout << "Privmsg command received:" << command.params[0] << std::endl;
	std::string target = command.params[0];
	std::string message = command.params[1];

	if (target.empty() || message.empty())
	{
		client.sendResponse(":server " + ServerMessages::ERR_NORECIPIENT + " * PRIVMSG :No recipient given");
		return;
	}

	// Vérifier la longueur maximale du message (RFC 2812 section 2.3)
	if (message.length() > 510) { // 512 - 2 (CRLF)
		client.sendResponse(":server " + ServerMessages::ERR_UNKNOWNCOMMAND + " * :Message too long");
		return;
	}

	// Support des messages multi-destinataires
	std::vector<std::string> targets = split(target, ',');
	for (size_t i = 0; i < targets.size(); ++i)
	{
		// ...traitement pour chaque destinataire...
	}

	if (target[0] == '#')
	{
		if (channels.find(target) == channels.end())
		{
			client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " * " + target + " :No such channel");
			return;
		}

		Channel *channel = channels[target];

		if (!channel->isMember(&client))
		{
			client.sendResponse(":server " + ServerMessages::ERR_NOTONCHANNEL + " * " + target + " :You're not on that channel");
			return;
		}

		std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + target + " :" + message;
		broadcast_message(target, msg, &client);
	}
	else
	{
		Client *targetClient = this->getClientByNickname(target);
		if (targetClient == NULL)
		{
			client.sendResponse(":server " + ServerMessages::ERR_NOSUCHNICK + " * " + target + " :No such nick");
			return;
		}

		std::string msg = ":" + client.getNickname() + "!" + client.getUsername() + "@localhost PRIVMSG " + target + " :" + message;
		targetClient->sendResponse(msg);
	}
}

void Server::handlePartCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (command.params.empty())
	{
		client.sendResponse(":server" + ServerMessages::ERR_NEEDMOREPARAMS + "PART :Not enough parameters");
		return;
	}
	std::cout << "PART command received: " << command.params[0] << std::endl;
	const std::string channelList = command.params[0];
	const std::string reason = (command.params.size() > 1) ? command.params[1] : "Leaving";

	std::vector<std::string> channelsToPart = split(channelList, ',');
	for (size_t i = 0; i < channelsToPart.size(); i++)
	{
		std::string channelName = channelsToPart[i];

		if (channels.find(channelName) != channels.end())
		{
			if (!channels[channelName]->isMember(&client))
			{
				client.sendResponse(":server" + ServerMessages::ERR_NOTONCHANNEL + channelName + " :You're not on that channel");
				continue;
			}
			// Annoncer le départ aux autres membres
			std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() +
								  "@localhost PART " + channelName + " :" + reason;
			broadcast_message(channelName, partMsg, NULL);
			client.sendResponse(partMsg);
			channels[channelName]->removeMember(&client);

			// Si le canal est vide, le supprimer
			if (channels[channelName]->getMembers().empty())
			{
				delete_channel(channelName);
			}
		}
		else
		{
			client.sendResponse(":server" + ServerMessages::ERR_NOSUCHCHANNEL + channelName + " :No such channel");
		}
	}
}

void Server::handleQuitCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	std::string tmp = command.params.empty() ? "nothing to say..." : command.params[0];
	std::string notification = ":" + client.getNickname() + "!" + client.getUsername() + " QUIT :" + tmp;
	std::cout << "QUIT command received: " << tmp << std::endl;

	const std::vector<Channel *> &clientChannels = client.getChannels(*this);
	for (std::vector<Channel *>::const_iterator it = clientChannels.begin(); it != clientChannels.end(); ++it)
	{
		this->broadcast_message((*it)->getName(), notification, &client);
		(*it)->removeMember(&client);
		if ((*it)->getMembers().empty())
		{
			this->delete_channel((*it)->getName());
		}
	}
	std::cout << client.getNickname() << " has quit the server.\n";
	this->remove_client(client.getFD());
}

void Server::handleWhoCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	// Si pas de paramètre, lister tous les utilisateurs visibles
	std::cout << "WHO command received with params: " << (command.params.empty() ? "none" : command.params[0]) << std::endl;
	if (command.params.empty())
	{
		// Parcourir tous les clients connectés
		for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
		{
			Client *target = it->second;
			if (target->getState() == Client::REGISTERED) // Ne lister que les utilisateurs enregistrés
			{
				// Format: "<client> <user> <host> <server> <nick> <H|G> :<hopcount> <real name>"
				client.sendResponse(":server " + ServerMessages::RPL_WHOREPLY + " " + client.getNickname() + " * " +
									target->getUsername() +			 // username
									" " + "localhost" +				 // host
									" irc.server" +					 // server
									" " + target->getNickname() +	 // nickname
									" H" +							 // Here (H) or Gone (G)
									" :0 " + target->getUsername()); // hopcount et realname
				std::cout << client.getNickname() << " * " << target->getUsername() << " localhost irc.server " << target->getNickname() << " H :0 " << target->getUsername() << std::endl;
			}
		}
		client.sendResponse(":server " + ServerMessages::RPL_ENDOFWHO + " " + client.getNickname() + " :End of WHO list");
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
				client.sendResponse(":server " + ServerMessages::RPL_WHOREPLY + " " + client.getNickname() + " " + target +
									" " + (*it)->getUsername() +	// username
									" localhost" +					// host
									" irc.server" +					// server
									" " + (*it)->getNickname() +	// nickname
									" H" + prefix +					// Here + operator status
									" :0 " + (*it)->getUsername()); // hopcount + realname
				std::cout << client.getNickname() << " " << target << " " << (*it)->getUsername() << " localhost irc.server " << (*it)->getNickname() << " H :0 " << (*it)->getUsername() << std::endl;
			}

			// Message de fin
			client.sendResponse(":server " + ServerMessages::RPL_ENDOFWHO + " " + client.getNickname() + " " + target + " :End of WHO list");
		}
		else
		{
			client.sendResponse(":server " + ServerMessages::ERR_NOSUCHCHANNEL + " " + client.getNickname() + " " + target + " :No such channel");
		}
	}
}
