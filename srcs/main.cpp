/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:23 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 15:57:55 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandParser.hpp"
#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"
#include <iostream>
#include <map>

std::map<std::string, Channel> channels;

void handleCommand(const CommandParser::ParsedCommand& command, Client& client)
{
    if (command.command == "NICK")
    {
        client.setNickname(command.params[0]);
    }
    else if (command.command == "USER")
    {
        client.setUsername(command.params[0]);
    }
    else if (command.command == "JOIN")
    {
        if (command.params.size() < 1)
        {
            throw std::invalid_argument("JOIN command requires a channel name");
        }
        const std::string& channelName = command.params[0];
        if (channels.find(channelName) == channels.end())
        {
            channels[channelName] = Channel(channelName);
        }
        channels[channelName].addMember(&client);
    }
    else if (command.command == "PRIVMSG")
    {
        std::string channelName = command.params[0];
        std::string message = command.params[1];
        if (channels.find(channelName) != channels.end())
        {
            Channel& channel = channels[channelName];
            const std::vector<Client*>& members = channel.getMembers();
            for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
            {
                Client* member = *it;
                std::cout << "Message to " << member->getNickname() << ": " << message << "\n";
            }
        }
    }
}

int main()
{
    // Faux client.
    Client client(42);
    try
    {
        client.setNickname("JohnDoe");
        client.setUsername("johndoe");
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    client.authenticate();

    // Simulation de commandes
    std::vector<std::string> rawMessages;
    rawMessages.push_back(":JohnDoe NICK JohnDoe");
    rawMessages.push_back(":JohnDoe USER johndoe");
    rawMessages.push_back(":JohnDoe JOIN #general");
    rawMessages.push_back(":JohnDoe PRIVMSG #general :Hello everyone!");

    for (std::vector<std::string>::const_iterator it = rawMessages.begin(); it != rawMessages.end(); ++it)
    {
        const std::string& rawMessage = *it;
        CommandParser::ParsedCommand parsed = CommandParser::parse(rawMessage);
        handleCommand(parsed, client);
    }

    // Vérification des résultats
    std::cout << "Client Nickname: " << client.getNickname() << "\n";
    std::cout << "Client Username: " << client.getUsername() << "\n";
    std::cout << "Authenticated: " << (client.authenticated() ? "Yes" : "No") << "\n";

    if (channels.find("#general") != channels.end())
    {
        Channel& generalChannel = channels["#general"];
        const std::vector<Client*>& members = generalChannel.getMembers();
        std::cout << "Members of #general:\n";
        for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            Client* member = *it;
            std::cout << "- " << member->getNickname() << "\n";
        }
    }

    return 0;
}


// #include "../includes/CommandParser.hpp"
// #include "../includes/Client.hpp"
// #include "../includes/Channel.hpp"
// #include <iostream>
// #include <map>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// std::map<std::string, Channel> channels;

// void handleCommand(const CommandParser::ParsedCommand& command, Client& client)
// {
//     if (command.command == "NICK")
//     {
//         client.setNickname(command.params[0]);
//     }
//     else if (command.command == "USER")
//     {
//         client.setUsername(command.params[0]);
//     }
//     else if (command.command == "JOIN")
//     {
//         if (command.params.size() < 1)
//         {
//             throw std::invalid_argument("JOIN command requires a channel name");
//         }
//         const std::string& channelName = command.params[0];
//         if (channels.find(channelName) == channels.end())
//         {
//             channels[channelName] = Channel(channelName);
//         }
//         channels[channelName].addMember(&client);
//     }
//     else if (command.command == "PRIVMSG")
//     {
//         std::string channelName = command.params[0];
//         std::string message = command.params[1];
//         if (channels.find(channelName) != channels.end())
//         {
//             Channel& channel = channels[channelName];
//             const std::vector<Client*>& members = channel.getMembers();
//             for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
//             {
//                 Client* member = *it;
//                 std::cout << "Message to " << member->getNickname() << ": " << message << "\n";
//             }
//         }
//     }
// }

// int main()
// {
//     // Création d'un client simulé
//     Client client(42);
//     try
//     {
//         client.setNickname("JohnDoe");
//         client.setUsername("johndoe");
//     }
//     catch (const std::exception& e)
//     {
//         std::cerr << "Error: " << e.what() << "\n";
//         return 1;
//     }
//     client.authenticate();

//     // Connexion au serveur
//     int sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (sock < 0)
//     {
//         std::cerr << "Error: Could not create socket\n";
//         return 1;
//     }

//     struct sockaddr_in server;
//     server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse IP du serveur
//     server.sin_family = AF_INET;
//     server.sin_port = htons(6667); // Port du serveur

//     if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
//     {
//         std::cerr << "Error: Could not connect to server\n";
//         return 1;
//     }

//     // Simulation de commandes
//     std::vector<std::string> rawMessages;
//     rawMessages.push_back(":JohnDoe NICK JohnDoe");
//     rawMessages.push_back(":JohnDoe USER johndoe");
//     rawMessages.push_back(":JohnDoe JOIN #general");
//     rawMessages.push_back(":JohnDoe PRIVMSG #general :Hello everyone!");

//     for (std::vector<std::string>::const_iterator it = rawMessages.begin(); it != rawMessages.end(); ++it)
//     {
//         const std::string& rawMessage = *it;
//         CommandParser::ParsedCommand parsed = CommandParser::parse(rawMessage);
//         handleCommand(parsed, client);

//         // Envoyer la commande au serveur
//         if (send(sock, rawMessage.c_str(), rawMessage.length(), 0) < 0)
//         {
//             std::cerr << "Error: Could not send message\n";
//             return 1;
//         }
//     }

//     // Vérification des résultats
//     std::cout << "Client Nickname: " << client.getNickname() << "\n";
//     std::cout << "Client Username: " << client.getUsername() << "\n";
//     std::cout << "Authenticated: " << (client.authenticated() ? "Yes" : "No") << "\n";

//     if (channels.find("#general") != channels.end())
//     {
//         Channel& generalChannel = channels["#general"];
//         const std::vector<Client*>& members = generalChannel.getMembers();
//         std::cout << "Members of #general:\n";
//         for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
//         {
//             Client* member = *it;
//             std::cout << "- " << member->getNickname() << "\n";
//         }
//     }

//     close(sock);
//     return 0;
// }
