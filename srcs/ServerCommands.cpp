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
            std::cout << "CAP LS sent" << std::endl;
            return;
        }
        if (command.command == "PASS")
        {
            std::cout << "is Authenticated: " << client.authenticated() << std::endl;
            handlePassCommand(command, client);
            std::cout << "Pass command received: " << command.params[0] << std::endl;
            std::cout << "is Authenticated: " << client.authenticated() << std::endl;
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
            std::cout << "Nick after: " << client.getNickname() << std::endl;
        }
        else if (command.command == "USER")
        {
            std::cout << "User before: " << client.getUsername() << std::endl;
            handleUserCommand(command, client);
            std::cout << "User command received:" << command.params[0] << std::endl;
            std::cout << "User after: " << client.getUsername() << std::endl;
        }
        else
            client.sendResponse("451 :You have not registered");
    }
    else if (client.getState() == Client::REGISTERED)
    {
        if (command.command == "NICK")
            handleNickCommand(command, client);
        else if (command.command == "JOIN")
            handleJoinCommand(command, client);
        else if (command.command == "MODE")
            handleModeCommand(command, client);
        else if (command.command == "WHO")
            handleWhoCommand(command, client);
        else if (command.command == "PRIVMSG")
            handlePrivmsgCommand(command, client);
        else if (command.command == "PART")
            handlePartCommand(command, client);
        else if (command.command == "QUIT")
            handleQuitCommand(command, client);
        else
            client.sendResponse(":server 421 * " + command.command + " :Unknown command");
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
    try {
        if (command.params.empty() || command.params[0].empty())
        {
            client.sendResponse(":server 431 * :No nickname given - Usage: NICK <nickname>");
            return;
        }
        
        std::string nick = command.params[0];
        std::cout << "Attempting to set nickname: '" << nick << "'" << std::endl;
        
        if (!this->isValidNickname(nick))
        {
            client.sendResponse(":server 432 * :Invalid nickname '" + nick + 
                              "'. Nickname must start with a letter, be 1-9 chars long, and use only letters, numbers, or -[]`^{}");
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
                client.sendResponse(":server 433 * " + originalNick + 
                                  " :Nickname is already in use and alternative too long. Please choose another");
                return;
            }
        }

        std::string oldNick = client.getNickname().empty() ? "*" : client.getNickname();
        client.setNickname(nick);
        
        if (oldNick == "*") {
            client.sendResponse(":server NOTICE Auth :Nickname set to " + nick + 
                              (nick != originalNick ? " (original choice was taken)" : ""));
        } else {
            client.sendResponse(":" + oldNick + " NICK :" + nick);
        }
        
        std::cout << "Nickname set to: '" << nick << "'" << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error in handleNickCommand: " << e.what() << std::endl;
        client.sendResponse(":server 432 * :Error setting nickname: " + std::string(e.what()));
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
    
    if (channelName.length() > 50)
    {
        std::cout << "Channel name too long" << std::endl;
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
    if (command.params.empty())
    {
        client.sendResponse(":server 461 " + client.getNickname() + " JOIN :Not enough parameters");
        return;
    }

    const std::string &channelName = command.params[0];
    
    // Vérifier la validité du nom du canal
    if (!isValidChannelName(channelName))
    {
        if (channelName[0] != '#')
        {
            client.sendResponse(":server 403 " + client.getNickname() + " " + channelName + 
                " :No such channel - Channel name must start with '#'");
        }
        else
        {
            client.sendResponse(":server 403 " + client.getNickname() + " " + channelName + 
                " :Invalid channel name");
        }
        return;
    }

    std::cout << "Attempting to join channel: " << channelName << std::endl;

    // Créer ou rejoindre le canal
    if (channels.find(channelName) == channels.end())
    {
        std::cout << "Creating new channel: " << channelName << std::endl;
        channels[channelName] = new Channel(channelName, &client);
        
        // Annoncer la création et le topic du canal
        std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@server JOIN " + channelName;
        broadcast_message(channelName, joinMsg);
        
        // Topic message
        client.sendResponse(":server 332 " + client.getNickname() + " " + channelName + " :Welcome to " + channelName);
        client.sendResponse(":server TOPIC " + channelName + " :Welcome to " + channelName);
        
        // Liste des membres et informations du canal
        client.sendResponse(":server 353 " + client.getNickname() + " = " + channelName + " :@" + client.getNickname());
        client.sendResponse(":server 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.");
    }
    else
    {
        Channel *channel = channels[channelName];
        channel->addMember(&client);
        
        // Annoncer l'arrivée du nouveau membre à tous les membres sauf lui-même
        std::string joinMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@server JOIN " + channelName;
        broadcast_message(channelName, joinMsg, &client);
        
        // Envoyer la liste complète des membres au nouveau membre
        std::string memberList = ":server 353 " + client.getNickname() + " = " + channelName + " :";
        const std::vector<Client *> &members = channel->getMembers();
        for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            if (channel->isOperator(*it))
                memberList += "@";
            memberList += (*it)->getNickname() + " ";
        }
        client.sendResponse(memberList);
        client.sendResponse(":server 366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.");
        
        // Message de bienvenue pour le nouveau membre
        std::string welcomeMsg = ":server NOTICE " + channelName + " :Welcome " + client.getNickname() + " to " + channelName + "!";
        broadcast_message(channelName, welcomeMsg);
    }

    // Afficher l'état du canal
    Channel *channel = channels[channelName];
    std::cout << "\nChannel state for " << channelName << ":" << std::endl;
    std::cout << "Number of members: " << channel->getMembers().size() << std::endl;
    std::cout << "Members: ";
    const std::vector<Client *> &members = channel->getMembers();
    for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        std::cout << (channel->isOperator(*it) ? "@" : "") << (*it)->getNickname() << " ";
    }
    std::cout << "\n" << std::endl;
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
        // Annoncer le départ aux autres membres
        std::string partMsg = ":" + client.getNickname() + "!" + client.getUsername() + "@server PART " + channelName;
        broadcast_message(channelName, partMsg);
        
        channels[channelName]->removeMember(&client);
        
        // Si le canal est vide, le supprimer
        if (channels[channelName]->getMembers().empty())
        {
            delete_channel(channelName);
        }
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

void Server::handleModeCommand(const CommandParser::ParsedCommand &command, Client &client)
{
    if (command.params.empty())
    {
        client.sendResponse(":server 461 " + client.getNickname() + " MODE :Not enough parameters");
        return;
    }

    std::string target = command.params[0];
    std::cout << "MODE request for target: " << target << std::endl;

    if (channels.find(target) != channels.end())
    {
        // Pas besoin de stocker channel si on ne l'utilise pas
        // Mode de base pour tous les canaux: +nt
        client.sendResponse(":server 324 " + client.getNickname() + " " + target + " +nt");
        
        // Temps de création du canal (en timestamp UNIX)
        std::stringstream ss;
        ss << time(NULL);
        client.sendResponse(":server 329 " + client.getNickname() + " " + target + " " + ss.str());
        
        std::cout << "Channel " << target << " modes: +nt" << std::endl;
    }
    else if (target == client.getNickname())
    {
        // Mode utilisateur par défaut: +i (invisible)
        client.sendResponse(":server 221 " + client.getNickname() + " +i");
        std::cout << "User " << target << " modes: +i" << std::endl;
    }
}

void Server::handleWhoCommand(const CommandParser::ParsedCommand &command, Client &client)
{
    if (command.params.empty())
    {
        client.sendResponse(":server 461 " + client.getNickname() + " WHO :Not enough parameters");
        return;
    }

    std::string target = command.params[0];
    std::cout << "WHO request for target: " << target << std::endl;

    if (channels.find(target) != channels.end())
    {
        Channel *channel = channels[target];
        const std::vector<Client *> &members = channel->getMembers();
        
        // Envoyer d'abord les informations du canal
        client.sendResponse(":server 324 " + client.getNickname() + " " + target + " Channel members:");
        
        for (std::vector<Client *>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            std::string prefix = channel->isOperator(*it) ? "@" : "";
            std::string roleSymbol = channel->isOperator(*it) ? "channel operator" : "member";
            
            // Format: "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
            client.sendResponse(":server 352 " + client.getNickname() + " " + target +
                            " " + (*it)->getUsername() +      // username
                            " " + target +                    // host
                            " irc.server" +                  // server
                            " " + (*it)->getNickname() +     // nickname
                            " H" + prefix +                  // Here + operator status
                            " :0 " + roleSymbol);           // hopcount + role
        }

        // Message de fin avec un résumé
        std::stringstream ss;
        ss << members.size();
        client.sendResponse(":server 315 " + client.getNickname() + " " + target + 
                          " :End of WHO list (" + ss.str() + " members)");
    }
    else
    {
        client.sendResponse(":server 403 " + client.getNickname() + " " + target + " :No such channel");
    }
}
