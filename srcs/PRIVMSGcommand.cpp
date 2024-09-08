#include "header.hpp"

bool Command::isChannel(const std::string &msg) const {
    return (msg[0] == '#' || msg[0] == '&');
}

bool Command::clientExist(std::string &name) const {
    return serverData.nameToClient.count(name);
}

bool    Command::duplicateExist(std::vector<std::string> &toSend) const {
    std::set <std::string> targets;
    for (size_t i = 0; i < toSend.size(); i++) {
        if (targets.count(toSend[i])) return 1;
        targets.insert(toSend[i]);
    }
    return 0;
}

Channel*    Command::getChannel(std::string name) const{
    std::deque<Channel*> &channels = serverData.channels;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->getName() == name)
        return channels[i];
    }
    return NULL;
}

void    Command::sendMsgToChannelClients(std::string &receiver, std::string &message) const {
    // check if channel exist : ERR_NOSUCHNICK 
    Channel *chan = getChannel(receiver.substr(1));
    if (!chan)
        sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), receiver));
    else {
        // check if client is on channel  : ERR_CANNOTSENDTOCHAN
        if (!chan->isOnChan(*client))
            //! ERR_CANNOTSENDTOCHAN
            sendMsg(client->getSocket(), ERR_CANNOTSENDTOCHAN(client->getNickName(), receiver));
        else {
            std::vector<Client> ChanClien = chan->getChanClients();
            for (size_t i = 0; i < ChanClien.size(); i++) {
                sendMsg(ChanClien[i].getSocket(), ":" + client->getNickName() + "!~" + client->getuserName() + "@"\
                    + client->getHostName() + " PRIVMSG " + receiver + " :" + message + "\r\n");
            }
        }
    }
}

void    Command::sendPrivMessage(std::vector<std::string> &toSend, std::string &message) const {
    if (toSend.size() > 10 || !toSend.size() || message.empty()) return ;
    if (duplicateExist(toSend)) {
        sendMsg(client->getSocket(), ERR_TOOMANYTARGETS(client->getNickName(), originCmd, 1));
        return ;
    }
    for(size_t i = 0; i < toSend.size(); i++) {
        std::string receiver = toSend[i];
        if (isChannel(receiver))
            sendMsgToChannelClients(receiver, message);
        else {
            // check if client is connected to the server
            if (clientExist(receiver)) {
                int receiverFd = serverData.nameToClient[receiver]->getSocket();
                sendMsg(receiverFd, ":" + client->getNickName() + "!~" + client->getuserName() + "@" + client->getHostName() + " PRIVMSG " + receiver + " :" + message + "\r\n");
            }
            else
                sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), receiver));
        }
    }
}

std::vector<std::string> Command::getUsersAndChannels() {
    std::vector<std::string> toSend;
    
    std::stringstream ss(cmd[1]);
    std::string newUser;
    while (getline(ss, newUser, ','))
        toSend.push_back(newUser);
    return toSend;
}

void    Command::executePrivmsg() {
    std::string messageTosSend = "";
    if (cmd.size() >= 3 && cmd[2][0] == ':') {
        size_t msgIdx = originCmd.find(':');
        messageTosSend = originCmd.substr(msgIdx + 1);
        originCmd.erase(originCmd.begin() + msgIdx, originCmd.end());
    }
    else if (cmd.size() == 3)
        messageTosSend = cmd[2];
    else {
        sendMsg(client->getSocket(), ERR_UNKNOWNCOMMAND(client->getNickName(), originCmd));
        return ;
    }
    std::vector<std::string> toSend = getUsersAndChannels();

    std::cout << messageTosSend << '\n';
    for(size_t i = 0; i < toSend.size(); i++)
        std::cout << toSend[i] << ' ';
    std::cout << '\n';
    if (toSend.empty())
        sendMsg(client->getSocket(), ERR_NORECIPIENT(client->getNickName()));
    if (messageTosSend.empty())
        sendMsg(client->getSocket(), ERR_NOTEXTTOSEND(client->getNickName()));
    if (toSend.size() > 10)
        sendMsg(client->getSocket(), ERR_TOOMANYTARGETS(client->getNickName(), originCmd, 0));
    sendPrivMessage(toSend, messageTosSend);
}