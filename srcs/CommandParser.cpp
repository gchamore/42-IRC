/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:58:18 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 16:19:08 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CommandParser.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>

CommandParser::ParsedCommand CommandParser::parse(const std::string& rawMessage)
{
    if (rawMessage.empty())
    {
        throw std::invalid_argument("Empty message");
    }
    std::istringstream iss(rawMessage);
    ParsedCommand command;
    std::string token;

    // Parse le prefix
    if (rawMessage[0] == ':')
    {
        iss >> token;
        command.prefix = token.substr(1);
    }

    // Parse la commande
    if (!(iss >> command.command))
    {
        throw std::invalid_argument("Invalid command");
    }

    // Vérification des commandes valides
    const std::string validCommandsArray[] = {"NICK", "USER", "JOIN", "PRIVMSG", "COMMAND"};
    const std::vector<std::string> validCommands(validCommandsArray, validCommandsArray + sizeof(validCommandsArray) / sizeof(validCommandsArray[0]));
    if (std::find(validCommands.begin(), validCommands.end(), command.command) == validCommands.end())
    {
        throw std::invalid_argument("Invalid command");
    }

    // Parse les parameters
    while (iss >> token)
    {
        if (token[0] == ':')
        {
            std::string trailing;
            std::getline(iss, trailing);
            command.params.push_back(token.substr(1) + trailing);
            break;
        } else
        {
            command.params.push_back(token);
        }
    }
    return command;
}
