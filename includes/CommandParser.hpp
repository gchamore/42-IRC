/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:57:55 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/30 15:30:39 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include "../includes/Debug.hpp"
#include <string>
#include <vector>
#include <stdexcept>

class CommandParser
{
public:
    // RFC 2812 Constants
    static const size_t MAX_PARAMS = 15;
    static const size_t MAX_PREFIX_LEN = 512;
    static const size_t MAX_COMMAND_LEN = 510;

    class ParseError : public std::runtime_error
    {
        public:
            explicit ParseError(const std::string& msg);
    };

    struct ParsedCommand
    {
        struct Prefix
        {
            std::string nickname;
            std::string username;
            std::string hostname;
            bool empty() const;
            bool operator==(const Prefix& other) const;
        };
        Prefix prefix;
        std::string command;
        std::vector<std::string> params;
    };

    static std::vector<CommandParser::ParsedCommand> parse(const std::string& rawMessage);
private:
    static void parsePrefix(const std::string& raw, ParsedCommand::Prefix& prefix);
    static bool isValidCommand(const std::string& cmd);
    static bool isValidParams(const std::vector<std::string>& params);
    static bool hasValidCRLF(const std::string& message);
};

#endif