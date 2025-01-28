#include "../includes/Server.hpp"

void Server::handleCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (client.getState() == Client::REGISTERING)
	{
		if (command.command == "PASS")
		{
			handlePassCommand(command, client);
		}
		else if (command.command == "NICK")
		{
			handleNickCommand(command, client);
		}
		else if (command.command == "USER")
		{
			handleUserCommand(command, client);
		}
		else
		{
			client.sendResponse("451 :You have not registered");
		}
	}
	else if (client.getState() == Client::REGISTERED)
	{
		if (command.command == "NICK")
		{
			handleNickCommand(command, client);
		}
		else if (command.command == "JOIN")
		{
			handleJoinCommand(command, client);
		}
		else if (command.command == "PRIVMSG")
		{
			handlePrivmsgCommand(command, client);
		}
		else if (command.command == "PART")
		{
			handlePartCommand(command, client);
		}
		else if (command.command == "QUIT")
		{
			handleQuitCommand(command, client);
		}
		else
		{
			client.sendResponse("421 " + command.command + " :Unknown command");
		}
	}
}

void Server::handlePassCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (client.authenticated())
	{
		client.sendResponse("462 ERR_ALREADYREGISTERED :You may not reregister");
		return;
	}
	if (command.params.size() != 1)
	{
		client.sendResponse("461 PASS :Not enough parameters or too many parameters");
		return;
	}
	if (command.params[0] != server_password)
	{
		client.sendResponse("464 ERR_PASSWDMISMATCH :Password incorrect");
		return;
	}

	client.authenticate();
	client.sendResponse("381 :You have successfully authenticated");
}

bool Server::isValidNickname(const std::string &nickname)
{
	const std::string specialChars = "-[]`^{}";

	if (nickname.empty() || nickname.length() > 9)
		return false;

	if (!std::isalpha(nickname[0]))
		return false;

	for (size_t i = 1; i < nickname.length(); ++i)
	{
		char c = nickname[i];
		if (!std::isalnum(c) && specialChars.find(c) == std::string::npos)
		{
			return false;
		}
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
	if (command.params.size() != 1)
	{
		client.sendResponse("461 NICK :Not enough parameters or too many parameters");
		return;
	}
	if (!this->isValidNickname(command.params[0]))
	{
		client.sendResponse("432 ERR_ERRONEUSNICKNAME :Erroneous nickname");
		return;
	}
	if (this->isNicknameTaken(command.params[0]))
	{
		client.sendResponse("433 ERR_NICKNAMEINUSE :Nickname is already in use");
		return;
	}

	client.setNickname(command.params[0]);
}

bool Server::isValidUsername(const std::string &username)
{
    // Check length (1–10 characters)
    if (username.empty() || username.length() > 10)
        return false;

    // Check allowed characters
    for (size_t i = 0; i < username.length(); ++i)
    {
        char c = username[i];
        if (!std::isalnum(c) && c != '-' && c != '.' && c != '_' && c != '~')
        {
            return false;
        }
    }

    return true;
}

void Server::handleUserCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (client.getState() != Client::REGISTERING)
	{
		client.sendResponse("462 ERR_ALREADYREGISTERED :You may not reregister");
		return;
	}
	if (client.getNickname().empty())
	{
		client.sendResponse("451 ERR_NOTREGISTERED :You have not registered");
		return;
	}
	if (command.params.size() != 1) // if (command.params.size() < 4)
	{
		client.sendResponse("461 USER :Not enough parameters or too many parameters");
		return;
	}
	if (!this->isValidUsername(command.params[0]))
	{
		client.sendResponse("432 ERR_ERRONEUSUSERNAME :Erroneous username");
		return;
	}
	client.setUsername(command.params[0]);
	client.setState(Client::REGISTERED);
	client.sendResponse("001 " + client.getNickname() + " :Welcome to the IRC Network!");
}

void Server::handleJoinCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (command.params.size() != 1)
	{
		client.sendResponse("461 JOIN :Not enough parameters or too many parameters");
		return;
	}
	const std::string &channelName = command.params[0];
	if (channels.find(channelName) == channels.end())
	{
		channels[channelName] = new Channel(channelName, &client);
	}
	channels[channelName]->addMember(&client);
}

void Server::handlePrivmsgCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (command.params.size() != 2)
	{
		client.sendResponse("461 PRIVMSG :Not enough parameters or too many parameters");
		return;
	}
	std::string channelName = command.params[0];
	std::string message = command.params[1];
	if (channels.find(channelName) != channels.end())
	{
		Channel *channel = channels[channelName];
		const std::vector<Client *> &members = channel->getMembers();
		for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
		{
			Client *member = *it;
			std::cout << "Message to " << member->getNickname() << ": " << message << "\n";
		}
	}
}

void Server::handlePartCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (command.params.size() < 1)
	{
		client.sendResponse("461 PART :Not enough parameters or too many parameters");
		return;
	}
	const std::string &channelName = command.params[0];
	if (channels.find(channelName) != channels.end())
	{
		channels[channelName]->removeMember(&client);
	}
}

void Server::handleQuitCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	std::string tmp = command.params.empty() ? "nothing to say..." : command.params[0];
	std::string notification = ":" + client.getNickname() + "!" + client.getUsername() + " QUIT :" + tmp;
	
	const std::vector<Channel *> &clientChannels = client.getChannels(*this);
	for (std::vector<Channel *>::const_iterator it = clientChannels.begin(); it != clientChannels.end(); ++it)
	{
		this->broadcast_message((*it)->getName(), notification);
		(*it)->removeMember(&client);
		if ((*it)->getMembers().empty())
		{
			this->delete_channel((*it)->getName());
		}
	}
	std::cout << client.getNickname() << " has quit the server.\n";
	this->remove_client(client.getFD());
}
