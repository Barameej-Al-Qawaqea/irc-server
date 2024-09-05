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
    int sendReturn = 1;
    if (client->isAlreadyRegistred())
        sendReturn &= sendMsg(client->getSocket(), ERR_ALREADYREGISTRED(client->getNickName())) != -1;
    else if (cmd.size() == 1) {
        sendReturn &= sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS((string)"*", (string)"PASS")) != -1;
        client->unsetAuthenticated();  // only last pass command is used for verif
    }
    else if (cmd.size() > 2 || cmd[1] != serverData.password) {
        sendReturn &= sendMsg(client->getSocket(), ERR_PASSWDMISMATCH((string)"*")) != -1;
        // this is according to rfc1459, only last pass command is used for verification
        client->unsetAuthenticated(); 
    }
    else
        client->setAuthenticated();
    if (!sendReturn)
        std::cerr << "Error occurs while sending message to the client\n";
}

void    Command::executeNick() {
    int sendReturn = 1;
    std::string destination = client->getNickName().empty() ? "*" : client->getNickName();   // destination should be '*' if user doenst have a nickname yet
    if (cmd.size() == 1)
        sendReturn &= sendMsg(client->getSocket(), ERR_NONICKNAMEGIVEN(destination)) != -1;
    else if (cmd.size() > 2 || !validNickName(cmd[1]))
        sendReturn &= sendMsg(client->getSocket(), ERR_ERRONEUSNICKNAME(destination)) != -1;
    else if (serverData.clientsNicknames.count(cmd[1])) // already inuse
        sendReturn &= sendMsg(client->getSocket(), ERR_NICKNAMEINUSE(destination)) != -1;
    else if (!client->getAuthenticated()) // no PASS YEt
        sendReturn &= sendMsg(client->getSocket(), ERR_NOTREGISTERED(destination)) != -1;
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
        sendReturn += sendMsg(client->getSocket(), cmd[1] + " successfully set a new nickname\n") != -1;
    }
    if (!sendReturn)
        std::cerr << "Error occurs while sending message to the client\n";
}

void    Command::executeUser() {
    int sendReturn = 1;
    std::string destination = client->getNickName().empty() ? "*" : client->getNickName();
    if (cmd.size() != 5)
        sendReturn &= sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(destination, "USER"));
    else if (client->isAlreadyRegistred()) 
        sendReturn &= sendMsg(client->getSocket(), ERR_ALREADYREGISTRED(destination));
    else if (!client->getAuthenticated() || client->getNickName().empty())  // user didnt make a PASS,NICK yet
        sendReturn &= sendMsg(client->getSocket(),  ERR_NOTREGISTERED(destination));
    else {
        // all good
        client->setRegistred();
        sendMsg(client->getSocket(),  "Welcome " + client->getNickName() + " to ft_irc server\n");
        serverData.nameToClient[client->getNickName()] = client;
    }   
}
