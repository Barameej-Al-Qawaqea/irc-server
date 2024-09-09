#include "header.hpp"
Client::Client(sockaddr_in &clientAddr, int clientSocket) {
    this->isAuthenticated = 0;
    this->isRegistred = 0;
    this->Addr = clientAddr;
    this->fd = clientSocket;
    this->currChan = NULL;
}


void    Client::setcurrChan(Channel *chan){
    currChan = chan;
}

Channel* Client::getcurrChan() const {
    return this->currChan;
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

bool Client::operator==(Client _client){
    return (_client.fd == this->fd);
}
std::string Client::play(std::vector<std::string> cmd){
    return bot.play(cmd);
}
std::string Client::botUsage(){
    return bot.botUsage();
}

bool    Client::argumentsError(std::vector<std::string> _cmd){
    return bot.argumentsError(_cmd);
}

Client::~Client() {}