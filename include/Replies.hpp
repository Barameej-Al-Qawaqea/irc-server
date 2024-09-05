#pragma once
#include "header.hpp"
// replies
#define RPL_TOPIC(chan,topic) chan+ " :" + topic + "\r\n"
#define RPL_NOTOPIC(chan) chan + " :No topic is set\r\n"
#define RPL_INVITING(chan,nick) chan + " " + nick + "\r\n"
#define RPL_NAMREPLY(chan,clients) "353 " + chan + " :" + clients + "\r\n"
#define ERR_CHANNELISFULL(chan) "471 " + chan + " :Cannot join channel (+l)\r\n"
// errors
#define ERR_INVITEONLYCHAN(chan) "473 " + chan + " :Cannot join channel (+i)\r\n"
#define ERR_NOSUCHCHANNEL(chan) "403 " + chan + " :No such channel\r\n"
#define ERR_BADCHANNELKEY(chan) "475 " + chan + " :Cannot join channel (+k)\r\n"
#define ERR_CHANOPRIVSNEEDED(chan) "482 " + chan + " :You're not channel operator\r\n"
#define ERR_CHANNELISFULL(chan) "471 " + chan + " :Cannot join channel (+l)\r\n"
#define ERR_NEEDMOREPARAMS(command) "461 " + command + " :Not enough parameters\r\n"
#define ERR_USERONCHANNEL(chan,nick) "443 " + nick + " " + chan + " :is already on channel\r\n"
#define ERR_NOTONCHANNEL(chan) "442 " + chan + " :You're not on that channel\r\n"
#define ERR_USERNOTINCHANNEL(chan,nick) "441 " + nick + " " + chan + " :They aren't on that channel\r\n"
#define ERR_UNKNOWNMODE(char) "472 " + char + " :is unknown mode char to me\r\n"