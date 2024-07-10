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
public :
    Client() {
        this->isAuthenticated = 0;
        this->isRegistred = 0;
    }
    ~Client() {}
};

#endif