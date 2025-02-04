/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:57:55 by gchamore          #+#    #+#             */
/*   Updated: 2025/02/04 11:11:59 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include "Debug.hpp"
#include "ServerMessages.hpp"
#include "Constants.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

class CommandParser
{
public:
	class ParseError : public std::runtime_error
	{
	public:
		explicit ParseError(const std::string &msg);
	};

	struct ParsedCommand
	{
		struct Prefix
		{
			std::string nickname;
			std::string username;
			std::string hostname;
			bool empty() const;
			bool operator==(const Prefix &other) const;
		};
		Prefix prefix;
		std::string command;
		std::vector<std::string> params;
	};

	static std::vector<CommandParser::ParsedCommand> parse(const std::string &rawMessage);

private:
	static void parsePrefix(const std::string &raw, ParsedCommand::Prefix &prefix);
	static bool isValidCommand(const std::string &cmd);
	static bool isValidParams(const std::vector<std::string> &params);
	static bool isValidHostname(const std::string &hostname);
	static bool hasValidCRLF(const std::string &message);
};

#endif