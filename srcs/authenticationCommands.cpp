#include "header.hpp"
#include "Client.hpp"

bool Command::validNickName(std::string &name) const {
    std::string validCharacters = "-{}[]^`\\";
    bool validName = 1;
    for(size_t i = 0; i < name.size(); i++)
        validName &= (std::isalpha(name[i]) || validCharacters.find(name[i]) != std::string::npos);
    return validName;

}

void    Command::executePass() {
    if (client->isAlreadyRegistred() || !client->getNickName().empty())    // already made a NICK, or NICK/USER
        sendMsg(client->getSocket(), ERR_ALREADYREGISTRED(client->getNickName()));
    else if (cmd.size() == 1) {
        sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS((string)"*", (string)"PASS"));
        client->unsetAuthenticated();  // only last pass command is used for verif
    }
    else if (cmd.size() > 2 || cmd[1] != serverData.password) {
        sendMsg(client->getSocket(), ERR_PASSWDMISMATCH((string)"*"));
        // this is according to rfc1459, only last pass command is used for verification
        client->unsetAuthenticated(); 
    }
    else
        client->setAuthenticated();
}

void    Command::executeNick() {
    std::string target = client->getNickName().empty() ? "*" : client->getNickName();   // target should be '*' if user doenst have a nickname yet
    if (cmd.size() == 1)
        sendMsg(client->getSocket(), ERR_NONICKNAMEGIVEN(target));
    else if (cmd.size() > 2 || !validNickName(cmd[1]))
        sendMsg(client->getSocket(), ERR_ERRONEUSNICKNAME(target));
    else if (serverData.clientsNicknames.count(cmd[1])) // already inuse
        sendMsg(client->getSocket(), ERR_NICKNAMEINUSE(target));
    else if (!client->getAuthenticated()) // no PASS YEt
        sendMsg(client->getSocket(), ERR_NOTREGISTERED(target));
    else {
        if (serverData.clientsNicknames.count(client->getNickName()))
            serverData.clientsNicknames.erase(client->getNickName());
        serverData.clientsNicknames.insert(cmd[1]);
        // should change his name in nameToClient too if he is fully registered
        if (client->isAlreadyRegistred()) {
            serverData.nameToClient.erase(client->getNickName());
            serverData.nameToClient[cmd[1]] = client;
        }
        client->setNickName(cmd[1]);
        sendMsg(client->getSocket(), cmd[1] + " successfully set a new nickname\n");
    }
}

void    Command::executeUser() {
    std::string target = client->getNickName().empty() ? "*" : client->getNickName();
    if (cmd.size() != 5)
        sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(target, "USER"));
    else if (client->isAlreadyRegistred()) 
        sendMsg(client->getSocket(), ERR_ALREADYREGISTRED(target));
    else if (!client->getAuthenticated() || client->getNickName().empty())  // user didnt make a PASS,NICK yet
        sendMsg(client->getSocket(),  ERR_NOTREGISTERED(target));
    else {
        // all good
        client->setRegistred();
        sendMsg(client->getSocket(),  "Welcome " + client->getNickName() + " to ft_irc server\n");
        serverData.nameToClient[client->getNickName()] = client;
    }   
}