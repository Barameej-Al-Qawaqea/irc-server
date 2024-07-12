#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "header.hpp"

#define NICKMAXLEN 9

class Client {
private :
    std::string nickname;
    std::string userName;
    bool isAuthenticated;   // to check if user already make a PASS
    bool isRegistred;      // user made PASS, NICK, USER
    sockaddr_in Addr; // client info
    int fd; // client socket
public :
    Client(sockaddr_in &clientAddr, int clientSocket) {
        this->isAuthenticated = 0;
        this->isRegistred = 0;
        this->Addr = clientAddr;
        this->fd = clientSocket;
    }
    // getters
    int getSocket() {
        return this->fd;
    }
    bool isAlreadyRegistred() {
        return this->isRegistred;
    }
    std::string getNickName() {
        return this->nickname;
    }

    //setters
    // client made succesfull PASS cmd
    void    setAuthenticated() {
        this->isAuthenticated = 1;
    }
    // client made successfull PASS,NICK,USER
    void    setRegistred() {
        this->isRegistred = 1;
    }
    ~Client() {}
};

#endif