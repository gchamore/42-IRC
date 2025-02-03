#ifndef SERVER_MESSAGES_HPP
#define SERVER_MESSAGES_HPP

#include <string>

namespace ServerMessages
{
    // Numeric Replies (001-099)
    const std::string RPL_WELCOME = "001";
    const std::string RPL_YOURHOST = "002";
    const std::string RPL_CREATED = "003";
    const std::string RPL_MYINFO = "004";

    // Command responses (200-399)
    const std::string RPL_WHOISUSER = "311";
    const std::string RPL_ENDOFWHO = "315";
    const std::string RPL_ENDOFWHOIS = "318";
    const std::string RPL_WHOISCHANNELS = "319";
    const std::string RPL_LIST = "322";
    const std::string RPL_LISTEND = "323";
    const std::string RPL_CHANNELMODEIS = "324";
    const std::string RPL_NOTOPIC = "331";
    const std::string RPL_TOPIC = "332";
    const std::string RPL_INVITING = "341";
    const std::string RPL_NAMREPLY = "353";
    const std::string RPL_ENDOFNAMES = "366";

    // WHO/WHOIS related responses (310-369)
    const std::string RPL_WHOREPLY = "352";      // "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
    const std::string RPL_WHOSPCRPL = "354";     // Format dépend des tokens demandés

    // MOTD related responses (370-399)
    const std::string RPL_MOTD = "372";          // ":- <text>"
    const std::string RPL_MOTDSTART = "375";     // ":- <server> Message of the day - "
    const std::string RPL_ENDOFMOTD = "376";     // ":End of MOTD command"

    // Error messages (400-599)
    const std::string ERR_NOSUCHNICK = "401";
    const std::string ERR_NOSUCHSERVER = "402";
    const std::string ERR_NOSUCHCHANNEL = "403";
    const std::string ERR_CANNOTSENDTOCHAN = "404";
    const std::string ERR_TOOMANYCHANNELS = "405";
    const std::string ERR_WASNOSUCHNICK = "406";
    const std::string ERR_NOORIGIN = "409";
    const std::string ERR_NORECIPIENT = "411";
    const std::string ERR_NOTEXTTOSEND = "412";
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
    const std::string ERR_BANNEDFROMCHAN = "474";
    const std::string ERR_BADCHANNELKEY = "475";
    const std::string ERR_CHANOPRIVSNEEDED = "482";

    const std::string ERR_UMODEUNKNOWNFLAG = "501";

    // Message templates
    namespace Templates
    {
        const std::string WELCOME = ":server " + RPL_WELCOME + " %s :Welcome to the IRC Network, %s!";
        const std::string NICK_IN_USE = ":server " + ERR_NICKNAMEINUSE + " * %s :Nickname is already in use";
        const std::string INVALID_NICK = ":server " + ERR_ERRONEUSNICKNAME + " * :Invalid nickname";
        const std::string NO_SUCH_CHANNEL = ":server " + ERR_NOSUCHCHANNEL + " %s :No such channel";
        const std::string NOT_ON_CHANNEL = ":server " + ERR_NOTONCHANNEL + " %s :You're not on that channel";
        const std::string NEED_MORE_PARAMS = ":server " + ERR_NEEDMOREPARAMS + " %s :Not enough parameters";
        const std::string CHANNEL_IS_FULL = ":server " + ERR_CHANNELISFULL + " %s :Cannot join channel (+l)";
        const std::string INVITE_ONLY = ":server " + ERR_INVITEONLYCHAN + " %s :Cannot join channel (+i)";
        const std::string BAD_CHANNEL_KEY = ":server " + ERR_BADCHANNELKEY + " %s :Cannot join channel (+k)";
        const std::string NOT_CHANNEL_OPERATOR = ":server " + ERR_CHANOPRIVSNEEDED + " %s :You're not channel operator";

        // WHO/WHOIS templates
        const std::string WHO_REPLY = ":server " + RPL_WHOREPLY + " %s %s %s %s %s H%s :0 %s";
        const std::string WHO_END = ":server " + RPL_ENDOFWHO + " %s :End of WHO list";
        const std::string NAMES_REPLY = ":server " + RPL_NAMREPLY + " = %s :%s";
        const std::string NAMES_END = ":server " + RPL_ENDOFNAMES + " %s :End of NAMES list";
    }
}

#endif
