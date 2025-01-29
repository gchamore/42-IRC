#include "../includes/Server.hpp"

void Server::handleCommand(const CommandParser::ParsedCommand &command, Client &client)
{
    // CAP & PASS command must be first for non-authenticated clients
    if (!client.authenticated())
    {
        if (command.command == "CAP")
        {
            std::cout << "CAP command received" << std::endl;
            client.sendResponse("CAP * LS :multi-prefix sasl");  // Standard CAP LS response
            std::cout << "CAP LS sent" << "\n" << std::endl;
            return;
        }
        if (command.command == "PASS")
        {
            std::cout << "is Authenticated: " << client.authenticated() << std::endl;
            handlePassCommand(command, client);
            std::cout << "Pass command received: " << command.params[0] << std::endl;
            std::cout << "is Authenticated: " << client.authenticated() << "\n" << std::endl;
        }
        return;  // Silently ignore other commands until authenticated
    }

    // Handle registration state commands
    if (client.getState() == Client::REGISTERING)
    {
        if (command.command == "NICK")
        {
            std::cout << "Nick before: " << client.getNickname() << std::endl;
            handleNickCommand(command, client);
            std::cout << "Nick command received:" << command.params[0] << std::endl;
            std::cout << "Nick after: " << client.getNickname() << "\n" << std::endl;
        }
        else if (command.command == "USER")
        {
            std::cout << "User before: " << client.getUsername() << std::endl;
            handleUserCommand(command, client);
            std::cout << "User command received:" << command.params[0] << std::endl;
            std::cout << "User after: " << client.getUsername() << "\n" << std::endl;
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
            client.sendResponse(":server 421 * " + command.command + " :Unknown command");
        }
    }
}

void Server::handlePassCommand(const CommandParser::ParsedCommand &command, Client &client)
{
    if (client.authenticated())
    {
        client.sendResponse(":server 462 * :You may not reregister");
        return;
    }

    if (command.params.empty())
    {
        client.sendResponse(":server 461 * PASS :Not enough parameters");
        return;
    }

    // Trim any whitespace from the password
    std::string provided_password = command.params[0];
    provided_password.erase(0, provided_password.find_first_not_of(" \t\r\n"));
    provided_password.erase(provided_password.find_last_not_of(" \t\r\n") + 1);

    if (provided_password != server_password)
    {
        client.sendResponse(":server 464 * :Password incorrect");
        return;
    }
    client.authenticate();
    client.sendResponse(":server NOTICE Auth :Password accepted");
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
    if (command.params.empty())
    {
        client.sendResponse(":server 431 * :No nickname given - Usage: NICK <nickname>");
        return;
    }
    
    std::string nick = command.params[0];
    if (!this->isValidNickname(nick))
    {
        client.sendResponse(":server 432 * :Invalid nickname '" + nick + 
                          "'. Nickname must start with a letter, be 1-9 chars long, and use only letters, numbers, or -[]`^{}");
        return;
    }
    
    if (this->isNicknameTaken(nick))
    {
        client.sendResponse(":server 433 * " + nick + 
                          " :Nickname is already in use. Please choose another");
        return;
    }

    std::string oldNick = client.getNickname().empty() ? "*" : client.getNickname();
    client.setNickname(nick);
    if (oldNick == "*") {
        client.sendResponse(":server NOTICE Auth :Nickname successfully set to " + nick);
    } else {
        client.sendResponse(":" + oldNick + " NICK :" + nick);
    }
}

bool Server::isValidUsername(const std::string &username)
{
    // Check length (1–10 characters)
    if (username.empty() || username.length() > 10)
        return false;

    // Check allowed characters
    for (size_t i = 0; username[i]; ++i)
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
        client.sendResponse(":server 462 * :You may not reregister");
        return;
    }
    if (!client.authenticated())
    {
        client.sendResponse(":server 464 * :You must send PASS first");
        return;
    }
    if (client.getNickname().empty())
    {
        client.sendResponse(":server 431 * :No nickname given");
        return;
    }

    // La commande USER nécessite 4 paramètres selon RFC 2812:
    // USER <username> <hostname> <servername> :<realname>
    if (command.params.size() < 4)
    {
        client.sendResponse(":server 461 * USER :Not enough parameters. Usage: USER <username> <hostname> <servername> :<realname>");
        return;
    }

    if (!this->isValidUsername(command.params[0]))
    {
        client.sendResponse(":server 432 * :Erroneous username");
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

void Server::handleJoinCommand(const CommandParser::ParsedCommand &command, Client &client)
{
	if (command.params.size() != 1)
	{
		client.sendResponse(":server 461 * JOIN :Not enough parameters");
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
		client.sendResponse(":server 461 * PRIVMSG :Not enough parameters");
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
		client.sendResponse(":server 461 * PART :Not enough parameters");
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
