/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 17:57:55 by gchamore          #+#    #+#             */
/*   Updated: 2025/01/23 14:33:28 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <string>
#include <vector>

class CommandParser
{
public:
    struct ParsedCommand
    {
        std::string prefix;
        std::string command;
        std::vector<std::string> params;
    };

    static ParsedCommand parse(const std::string& rawMessage);
};

#endif