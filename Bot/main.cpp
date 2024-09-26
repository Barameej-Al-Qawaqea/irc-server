#include "Bot.hpp"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdlib>
#include <poll.h>
#include <sstream>

#define MAX_BUFF_SIZE 64000

int connectionSetup(int port, std::string &ipAddress)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
	{
		std::cout << "createSocket()::creating socket Err" << std::endl;
		exit(0);
	}
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Connection to server failed\n" << std::endl;
        close(sockfd);
        exit(0);
    }
    return sockfd;
}

void    authenticate(int serverSocket, std::string &serverPassword, std::string &nickname) {
    // send PASS, NICK, USER to server
    std::string message = "PASS " + serverPassword + "\r\n";
    message += "NICK " + nickname + "\r\n";
    message += "USER * * * *\r\n";

    if (send(serverSocket, message.c_str(), message.size(), 0) < 0) {
        std::cerr << "Error in sending\n";
        close(serverSocket);
        exit(0);   
    }

    char *buff = new char[MAX_BUFF_SIZE + 1];

    int bytes = recv(serverSocket, buff, MAX_BUFF_SIZE, 0);
    if (bytes < 0) {
        std::cerr << "Error in reading "<< ' '<< bytes << "\n";
        exit(0);
    }
    buff[bytes] = 0;
    if (buff[0] != '0') {
        // some error happens;
        std::cout << "Couldn't connect to server\n";
        std::string error = buff;
        std::cout << "server Error -> " << error.substr(error.find(':') + 1, error.find('\r') - error.find(':') + 1) << '\n';
        close(serverSocket);
        exit(0);
    }
    else 
        std::cout << "the Bot is successfully connected to the server\n";
}

int getPortNumber(std::string &port) {
    // check if it is valid
	int number = 0;
    bool invalid = port.empty();
	for (int i = 0; i < int(port.size()); i++)
	{
		if (!std::isdigit(port[i])) {
            invalid = 1;
            break ;
        }
		number = number * 10 + (port[i] - '0');
		if (number > 65535) {
            invalid = 1;
            break ;
        };
	}
    if (invalid) {
        std::cout << "Invalid Port\n";
        exit(0);
    }
	return (number);
}

void    setSocketAsNonBlocking(int serverSocket)
{
	int status = fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	if (status < 0)
    {
		std::cerr << "setsockopt failed\n";
		close(serverSocket);
		exit(0);
	}
}

int establichServerConnection() {
    std::string serverPassword, port, ipAddress;

    std::cout << "Enter server Password :\n";
    std::getline(std::cin, serverPassword);

    std::cout << "Enter Port number :\n";
    std::getline(std::cin, port);
    int portNumber = getPortNumber(port);

    std::cout << "Enter server Ip address :\n";
    std::getline(std::cin, ipAddress);
    
    std::string nickName;
    std::cout << "Enter a nick name for the bot :\n";
    std::getline(std::cin, nickName);

    int serverSocket = connectionSetup(portNumber, ipAddress);
    authenticate(serverSocket, serverPassword, nickName);
    setSocketAsNonBlocking(serverSocket);
    return serverSocket;
}


bool    isPrivmsgCommand(std::string &cmd) {
    std::stringstream ss(cmd);
    std::string word;
    ss >> word >> word;
    return (word == "PRIVMSG");
}


std::string getMessage(std::string &cmd) {
    std::stringstream ss(cmd);
    std::string msg;
    ss >> msg >> msg >> msg; ss.ignore();
    std::getline(ss, msg);
    int msgIdx = msg.find(':') + 1;
    msg = msg.substr(msgIdx);
    return msg;
}


void    sendMessage(int serverSocket, std::string message, std::string &clientNickname) {
    std::stringstream ss(message);
    std::string line;
	while (std::getline(ss, line, '\n')) {
        std::string curMsg = "PRIVMSG "+ clientNickname + " :" + line + "\r\n";
        int sendStatus = send(serverSocket, curMsg.c_str(), curMsg.size(), 0);
	    if (sendStatus < 0) {
            std::cerr << "Error occurs while sending message\n";
            return ;
        }
    }
}

std::string setupNewLogin(int login, std::map<int, Bot*> &data) {
    if (data.count(login))
        return "this login is already used, please chose another one\n";
    data[login] = new Bot();
    return "you have succesfully setup a new login\n";
}

std::string    getResponce(std::vector<std::string> &cmds, std::map<int, Bot*> &data) {
    if (Bot::argumentsError(cmds))
        return Bot::botUsage();
    int login = std::atoi(cmds.back().c_str());
    if (cmds.size() == 2)
        return setupNewLogin(login, data);
    else {
        if (!data.count(login))
            return "No such login number found\n";
        cmds.pop_back();
        return data[login]->play(cmds);
    }
}

void    newCmnd(int serverSocket, std::string &cmd, std::map<int, Bot*> &data) {
    if (!isPrivmsgCommand(cmd)) return ;
    std::string clientName = cmd.substr(1, cmd.find('!') - 1);
    std::string msg = getMessage(cmd);
    std::stringstream ss(msg);
    std::vector<std::string> cmds;
    std::string word;
    while (ss >> word) {
        cmds.push_back(word);
    }

    std::string toSend = getResponce(cmds, data);
    sendMessage(serverSocket, toSend, clientName);
}

void    checkNewCommands(int serverSocket, std::string &cmnd, std::map<int, Bot*> &data) {
    if (cmnd.empty()) return;
    std::stringstream ss(cmnd);
    std::string sep = "\n";

    std::stringstream S(cmnd);
    std::string t;
    while (getline(S, t, '\n'))
    {
        while (!t.empty() && (t.back() == '\n' || t.back() == '\r' || t.back() == '\0')) t.pop_back();
        newCmnd(serverSocket, t, data);
    }
}


int main(void) {
    int serverSocket = establichServerConnection();
    struct pollfd fd;
    fd.revents = 0;
    fd.events = POLLIN;
    fd.fd = serverSocket;
    
    std::map<int, Bot*> data;
    char *buff = new char[MAX_BUFF_SIZE + 1];
    while (1) {
        poll(&fd, 1, -1);
        int bytesRead = recv(serverSocket, buff, MAX_BUFF_SIZE, 0);
	    if (bytesRead <= 0) {
            std::cout << "Server is down\n";
            close(serverSocket);
            return 0;
        }
	    buff[bytesRead] = '\0';
	    std::string response = std::string(buff, bytesRead);
        checkNewCommands(serverSocket, response, data);
    }
    for (std::map<int, Bot*>::iterator it = data.begin(); it != data.end(); it++) 
        delete it->second;
}
