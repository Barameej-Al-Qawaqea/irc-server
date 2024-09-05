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

bool    Command::sendMsgToChannelClients(std::string &receiver, std::string &message) const {
    bool sendReturn = 1;
    // check if channel exist : ERR_NOSUCHNICK 
    Channel *chan = getChannel(receiver.substr(1));
    if (!chan)
        sendReturn &= sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), receiver)) != -1;
    else {
        // check if client is on channel  : ERR_CANNOTSENDTOCHAN
        if (!chan->isOnChan(*client))
            //! ERR_CANNOTSENDTOCHAN
            sendReturn &= sendMsg(client->getSocket(), ERR_CANNOTSENDTOCHAN(client->getNickName(), receiver)) != -1;
        else {
            std::vector<Client> ChanClien = chan->getChanClients();
            for (size_t i = 0; i < ChanClien.size(); i++) {
                if (ChanClien[i].getSocket() == client->getSocket()) continue;    
                sendReturn &= sendMsg(ChanClien[i].getSocket(), "Message from " + client->getNickName() + ": " + message + "\n") != -1;
            }
        }
    }
    return sendReturn;
}

bool   Command::sendPrivMessage(std::vector<std::string> &toSend, std::string &message) const {
    if (toSend.size() > 10 || !toSend.size() || message.empty()) return 1;
    bool sendReturn = 1;
    if (duplicateExist(toSend))
        return sendMsg(client->getSocket(), ERR_TOOMANYTARGETS(client->getNickName(), originCmd, 1)) != -1;
    for(size_t i = 0; i < toSend.size(); i++) {
        std::string receiver = toSend[i];
        if (isChannel(receiver))
            sendReturn &= sendMsgToChannelClients(receiver, message);
        else {
            // check if client is connected to the server
            if (clientExist(receiver)) {
                int receiverFd = serverData.nameToClient[receiver]->getSocket();
                sendReturn &= sendMsg(receiverFd, "Message from " + client->getNickName() + ": " + message + "\n") != -1;
            }
            else
                sendReturn &= sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), receiver)) != -1;
        }
    }
    return sendReturn;
}

std::vector<std::string> Command::getUsersAndChannels() {
    std::vector<std::string> toSend;
    size_t i = 0;
    while (isspace(originCmd[i])) i++;
    while (i < originCmd.size() && !isspace(originCmd[i])) i++;
    while (i < originCmd.size() && isspace(originCmd[i])) i++;
    originCmd.erase(originCmd.begin(), originCmd.begin() + i);
    while (originCmd.size() && isspace(originCmd.back())) originCmd.pop_back();

    if (originCmd.empty()) return toSend;
    std::stringstream ss(originCmd);
    std::string newUser;
    while (getline(ss, newUser, ','))
        toSend.push_back(newUser);
    return toSend;
}

void    Command::executePrivmsg() {
    std::string messageTosSend = "";
    size_t msgIdx = originCmd.find(':');
    if (msgIdx != std::string::npos) {
        messageTosSend = originCmd.substr(msgIdx + 1);
        originCmd.erase(originCmd.begin() + msgIdx, originCmd.end());
    }
    std::vector<std::string> toSend = getUsersAndChannels();

    // std::cout << messageTosSend << '\n';
    // for(size_t i = 0; i < toSend.size(); i++)
    //     std::cout << toSend[i] << ' ';
    // std::cout << '\n';
    int sendReturn = 1;
    if (toSend.empty())
        sendReturn &= sendMsg(client->getSocket(), ERR_NORECIPIENT(client->getNickName())) != -1;
    if (messageTosSend.empty())
        sendReturn &= sendMsg(client->getSocket(), ERR_NOTEXTTOSEND(client->getNickName())) != -1;
    if (toSend.size() > 10)
        sendReturn &= sendMsg(client->getSocket(), ERR_TOOMANYTARGETS(client->getNickName(), originCmd, 0)) != -1;
    sendReturn &= sendPrivMessage(toSend, messageTosSend);
    if (!sendReturn)
        std::cerr << "Error occurs while sending message to the client\n";
}