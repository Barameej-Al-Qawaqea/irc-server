#ifndef REPLIES_HPP
# define REPLIES_HPP

#include "header.hpp"
// replies
#define RPL_TOPIC(chan,topic) chan+ " :" + topic + "\r\n"
#define RPL_JOIN(nick, user, ipaddress, chan) ":" + nick + "!~" + user + "@" + ipaddress + " JOIN #" + chan + "\r\n"
#define RPL_NOTOPIC(chan, nick) "331 " + nick + " " + chan + " :No topic is set\r\n"
#define RPL_INVITING(chan,nick, target) "341 " + nick + " " + target + " #" + chan + "\r\n"
#define RPL_CHANNELMODEIS(chan, _mode) "324 " + chan + " " + _mode + "\r\n"
#define RPL_PART(nick, user, ipaddress, chan) ":" + nick + "!~" + user + "@" + ipaddress + " PART #" + chan + "\r\n"
// #define RPL_NAMREPLY(chan,clients) "353 " + chan + " :" + clients + "\r\n"
#define ERR_CHANNELISFULL(nick, chan) "471 " + nick + " " + chan + " :Cannot join channel (+l)\r\n"
#define RPL_NAMREPLY(chan, nick,clients) "353 " + nick + " = #" + chan + " :@" + nick + "\r\n"
#define RPL_ENDOFNAMEST(chan, nick) "366 " + nick + " #" + chan + " :End of /NAMES list.\r\n"
// errors
#define ERR_INVITEONLYCHAN(nick ,chan) "473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_NOSUCHCHANNEL(nick, chan) "403 " + nick + " " + chan + " :No such channel\r\n"
#define ERR_BADCHANNELKEY(client, chan) "475 " + client + " " + chan + " :Cannot join channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED(nick, chan) "482 " + nick + " " + chan + " :You're not channel operator\r\n"
#define ERR_CHANNELISFULL(nick, chan) "471 " + nick + " " + chan + " :Cannot join channel (+l)\r\n"

#define ERR_USERONCHANNEL(chan,nick) "443 " + nick + " " + chan + " :is already on channel\r\n"
#define ERR_NOTONCHANNEL(nick, chan) "442 " + nick + " " + chan + " :You're not on that channel\r\n"
#define ERR_USERNOTINCHANNEL(chan,nick) "441 " + nick + " " + chan + " :They aren't on that channel\r\n"
#define ERR_UNKNOWNMODE(nick, mode) "472 " + nick + " " + mode + " :is unknown mode char to me\r\n"

// PASS
#define ERR_ALREADYREGISTRED(nick) ":server 462 " + nick + " :You You may not reregister\r\n"
#define ERR_NEEDMOREPARAMS(nick, command) ":server 461 " + nick + " " + command + " :Not enough parameters\r\n"
#define ERR_PASSWDMISMATCH(nick) ":server 464 " + nick + " :Password incorrect\r\n"
#define ERR_NOTREGISTERED(nick) ":server 451 " + nick + " :You have not registered\r\n"
#define ERR_UNKNOWNCOMMAND(nick, command) ":server 421 " + nick + " " + command + " :Unknown command\r\n"
// NICK
#define ERR_NONICKNAMEGIVEN(nick) ":server 431 " + nick + " :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick) ":server 432 " + nick + " :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE(nick) ":server 433 " + nick + " :Nickname is already in use\r\n"
#define RPL_NICKCHANGE(nick, oldNick) ":" + oldNick + " NICK " + nick + "\r\n"

// PRIVMSG : tocheck
#define ERR_NOSUCHNICK(nick, badNick) ":server 401 " + nick + " " + badNick + " :No such nick/channel\r\n"
#define ERR_TOOMANYTARGETS(nick, duplicates) ":server 407 " + nick + (duplicates == 1 ? " :Duplicate recipients. " : " :Too many recipients. ") + "No message delivered\r\n"
#define ERR_NORECIPIENT(nick) ":server 411 " + nick + " :No recipient given (PRIVMSG)\r\n"
#define ERR_NOTEXTTOSEND(nick) ":server 412 " + nick + " :No text to send\r\n"
#define ERR_CANNOTSENDTOCHAN(nick, channel) ":server 404 " + nick + " " + channel + " :Cannot send to channel\r\n"

//
#endif
