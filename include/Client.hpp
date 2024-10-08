#ifndef CLIENT_HPP
# define CLIENT_HPP
#include "header.hpp"

#define NICKMAXLEN 9
class Channel;
class Bot;
class Client {
private :
    std::string     nickName;
    std::string     userName;
    bool            isAuthenticated;   // to check if user already make a PASS
    bool            isRegistred;      // user made PASS, NICK, USER
    sockaddr_in     Addr; // client info
    int             fd; // client socket
    std::set<int>   activeChatsSockets; // use it to inform  NICK name changes to users who have chat with client

public :
    Client(sockaddr_in &clientAddr, int clientSocket);
    int             getSocket() const;
    bool            isAlreadyRegistred() const;
    bool            getAuthenticated() const;
    std::string     getNickName() const;
    std::string     getuserName() const;
    std::string     getHostName() const; 

    void            setAuthenticated();
    void            unsetAuthenticated();
    void            setRegistred();
    void            setUserName(std::string &name);
    void            setNickName(std::string &name);
    std::set<int>&  getActiveChatsSockets();
    void            addActiveChat(int clientSocket);
    void            deleteActiveChat(int clientSocket);
    bool            operator==(Client &_client);
    std::string     play(std::vector<std::string> cmd);
    std::string     botUsage() const;
    bool            argumentsError(std::vector<std::string> &cmd) const;
    ~Client();
};

#endif
