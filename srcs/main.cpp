/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:23 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/24 13:14:19 by gchamore         ###   ########.fr       */
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
    else if (command.command == "PART")
    {
        if (command.params.size() < 1)
        {
            throw std::invalid_argument("PART command requires a channel name");
        }
        const std::string& channelName = command.params[0];
        if (channels.find(channelName) != channels.end())
        {
            channels[channelName].removeMember(&client);
        }
    }
    else if (command.command == "QUIT")
    {
        std::cout << client.getNickname() << " has quit the server.\n";
        for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it)
        {
            it->second.removeMember(&client);
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
    rawMessages.push_back(":JohnDoe NICK JohnDoe\r\n");
    rawMessages.push_back(":JohnDoe USER johndoe\r\n");
    rawMessages.push_back(":JohnDoe JOIN #general\r\n");
    rawMessages.push_back(":JohnDoe PRIVMSG #general :Hello everyone!\r\n");

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

        // Quit the server
        CommandParser::ParsedCommand quitCommand = CommandParser::parse(":JohnDoe QUIT :Goodbye!\r\n");
        handleCommand(quitCommand, client);
        std::cout << "Members of #general:\n";
        for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
        {
            Client* member = *it;
            std::cout << "- " << member->getNickname() << "\n";
        }
    }

    return 0;
}
