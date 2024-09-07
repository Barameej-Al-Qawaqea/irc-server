#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "header.hpp"

#define NICKMAXLEN 9

class Channel;
class Bot;
class Client {
private :
    Channel *currChan;
    std::string nickName;
    std::string userName;
    bool isAuthenticated;   // to check if user already make a PASS
    bool isRegistred;      // user made PASS, NICK, USER
    sockaddr_in Addr; // client info
    int fd; // client socket
    Bot bot;
public :
    Client(sockaddr_in &clientAddr, int clientSocket);
    Channel*    getcurrChan() const; 
    int         getSocket() const;
    bool        isAlreadyRegistred() const;
    bool        getAuthenticated() const;
    std::string getNickName() const;

    void        setAuthenticated();
    void        unsetAuthenticated();
    void        setcurrChan(Channel *chan);
    void        setRegistred();
    void        setNickName(std::string &name);

    bool        operator==(Client _client);
    std::string play(std::vector<std::string> cmd);
    std::string botUsage();
    bool        argumentsError(std::vector<std::string> _cmd);
    std::string getuserName(){
        return userName;
    }
    std::string getHostName(){
        char hostname[1024];
        hostname[1023] = '\0';
        gethostname(hostname, 1023);

        // return std::string(hostname);
        return std::string("127.0.0.1");
    }

    void    setUserName(std::string &name) {
        userName = name;
    }
    ~Client();
};

#endif