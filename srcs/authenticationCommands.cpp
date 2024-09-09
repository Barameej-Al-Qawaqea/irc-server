
#include "header.hpp"
#include "Client.hpp"

bool Command::validNickName(std::string &name) const {
    std::string validCharacters = "_-{}[]^`\\";
    bool validName = 1;
    for(size_t i = 0; i < name.size(); i++)
        validName &= (std::isalpha(name[i]) || validCharacters.find(name[i]) != std::string::npos);
    validName &= (name.size() <= NICKMAXLEN);
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
        client->unsetAuthenticated(); 
    }
    else
        client->setAuthenticated();
}


void    Command::notifyNickChangeToChannels(const std::string &oldName, const std::string &newName) const{

    std::deque<Channel *> &channels = serverData.channels;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->isOnChan(client)) {
            vector<Client> ChannelClients = channels[i]->getChanClients();
            for (size_t i = 0; i < ChannelClients.size(); i++) {
                sendMsg(ChannelClients[i].getSocket(), RPL_NICKCHANGE(newName, oldName));
            }
        }
    }
}

void    Command::notifyActiveChats(const std::string &oldName, const std::string &newName) const{
    std::set<int> &activeChatsSockets = client->getActiveChatsSockets();
    
    for(auto it = activeChatsSockets.begin(); it != activeChatsSockets.end(); it++)
        sendMsg(*it, RPL_NICKCHANGE(newName, oldName));
}

void    Command::executeNick() {
    std::string target = client->getNickName().empty() ? "*" : client->getNickName();   // target should be '*' if user doenst have a nickname yet
    // std::cout << " coomand: " << originCmd << ' ' << cmd.size() << std::endl;
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
        // sendMsg(client->getSocket(), RPL_NICKCHANGE(cmd[1], (client->getNickName().size() ? client->getNickName() : "*")));
        // info
        notifyNickChangeToChannels(client->getNickName(), cmd[1]);
        notifyActiveChats(client->getNickName(), cmd[1]);
        client->setNickName(cmd[1]);
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
        sendMsg(client->getSocket(),  "001 " + client->getNickName() + " :Welcome to ft_irc server\r\n");
        serverData.nameToClient[client->getNickName()] = client;
        client->setUserName(cmd[1]);
    }   
}
