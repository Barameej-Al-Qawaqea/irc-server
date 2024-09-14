#include "header.hpp"
Client::Client(sockaddr_in &clientAddr, int clientSocket) {
    this->isAuthenticated = 0;
    this->isRegistred = 0;
    this->Addr = clientAddr;
    this->fd = clientSocket;
}



int Client::getSocket() const {
    return this->fd;
}

bool    Client::isAlreadyRegistred() const {
    return this->isRegistred;
}
bool    Client::getAuthenticated() const {
    return this->isAuthenticated;
}

std::string Client::getNickName() const {
    return this->nickName;
}

std::string Client::getuserName() const {
    return userName;
}

std::string Client::getHostName() const {
    return  inet_ntoa(Addr.sin_addr);
}

void    Client::setAuthenticated() {
    this->isAuthenticated = 1;
}

void    Client::unsetAuthenticated() {
    this->isAuthenticated = 0;
}

void    Client::setRegistred() {
    this->isRegistred = 1;
}
void    Client::setNickName(std::string &name) {
    this->nickName = name;
}

void    Client::setUserName(std::string &name) {
    userName = name;
}

void    Client::deleteActiveChat(int clientSocket) {
    activeChatsSockets.erase(clientSocket);
};

std::set<int>&  Client::getActiveChatsSockets() {
    return activeChatsSockets;
}

void    Client::addActiveChat(int clientSocket) {
    activeChatsSockets.insert(clientSocket);
}

bool    Client::operator==(Client &_client){
    return (_client.fd == this->fd);
}

std::string Client::play(std::vector<std::string> cmd){
    return bot.play(cmd);
}
std::string Client::botUsage() const {
    return bot.botUsage();
}

bool    Client::argumentsError(std::vector<std::string> &cmd) const {
    return bot.argumentsError(cmd);
}


Client::~Client() {}