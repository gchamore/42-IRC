/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerMessages.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gchamore <gchamore@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 12:01:51 by gchamore          #+#    #+#             */
/*   Updated: 2025/02/05 12:03:44 by gchamore         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MESSAGES_HPP
#define SERVER_MESSAGES_HPP

#include <string>

namespace ServerMessages
{
    // Numeric Replies (001-099)
    const std::string RPL_WELCOME = "001";

    // Command responses (200-399)
    const std::string RPL_ENDOFWHO = "315";
    const std::string RPL_CHANNELMODEIS = "324";
    const std::string RPL_NOTOPIC = "331";
    const std::string RPL_TOPIC = "332";
    const std::string RPL_INVITING = "341";
    const std::string RPL_NAMREPLY = "353";
    const std::string RPL_ENDOFNAMES = "366";

    // WHO/WHOIS related responses (310-369)
    const std::string RPL_WHOREPLY = "352";

    // Error messages (400-599)
    const std::string ERR_NOSUCHNICK = "401";
    const std::string ERR_NOSUCHCHANNEL = "403";
    const std::string ERR_TOOMANYCHANNELS = "405";
    const std::string ERR_NORECIPIENT = "411";
    const std::string ERR_UNKNOWNCOMMAND = "421";
    const std::string ERR_NONICKNAMEGIVEN = "431";
    const std::string ERR_ERRONEUSNICKNAME = "432";
    const std::string ERR_NICKNAMEINUSE = "433";
    const std::string ERR_USERNOTINCHANNEL = "441";
    const std::string ERR_NOTONCHANNEL = "442";
    const std::string ERR_USERONCHANNEL = "443";
    const std::string ERR_NOTREGISTERED = "451";
    const std::string ERR_NEEDMOREPARAMS = "461";
    const std::string ERR_ALREADYREGISTERED = "462";
    const std::string ERR_PASSWDMISMATCH = "464";
    const std::string ERR_CHANNELISFULL = "471";
    const std::string ERR_UNKNOWNMODE = "472";
    const std::string ERR_INVITEONLYCHAN = "473";
    const std::string ERR_BADCHANNELKEY = "475";
    const std::string ERR_CHANOPRIVSNEEDED = "482";

    const std::string ERR_UMODEUNKNOWNFLAG = "501";
}

#endif
