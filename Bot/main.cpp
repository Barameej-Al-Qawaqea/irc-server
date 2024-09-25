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
    std::cerr << port << ' ' << ipAddress << '\n';
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cout << "Connection to server failed\n" << std::endl;
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
        exit(0);
    }
    std::cout << buff << '\n';
}

int getPortNumber(std::string &port) {
    // check if it is valid

    return atoi(port.c_str());
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

std::string recvMsg(int serverSocket, char *buff)
{
	int recvStatus = recv(serverSocket, buff, MAX_BUFF_SIZE, 0);
	if (recvStatus <= 0) return std::string("");
	buff[recvStatus] = '\0';
	std::string rsp = std::string(buff);
	return (rsp);
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
    // either setup login
    // or game command login
}

void    checkNewCommands(int serverSocket, std::string &cmnd, std::map<int, Bot*> &data) {
    if (cmnd.empty()) return;
    std::cout << cmnd << std::endl;
    std::stringstream ss(cmnd);
    std::string sep = "\n";

    if (cmnd.size() > 1 && cmnd[cmnd.size() - 2] == '\r') sep = "\r\n";   // not from terminal
    size_t end, start = 0;
    while ((end = cmnd.find(sep, start)) != std::string::npos) {
        std::string subCmd = cmnd.substr(start, end - start);
        newCmnd(serverSocket, subCmd, data);
        start = end + sep.size();
    }
}


int main(void) {
    int serverSocket = establichServerConnection();
    std::map<int, Bot*> data;
    char *buff = new char[MAX_BUFF_SIZE + 1];
    while (1) {
        int recvStatus = recv(serverSocket, buff, MAX_BUFF_SIZE, 0);
	    if (recvStatus <= 0) continue;
	    buff[recvStatus] = '\0';
	    std::string response = std::string(buff);
        checkNewCommands(serverSocket, response, data);
    }
    for (std::map<int, Bot*>::iterator it = data.begin(); it != data.end(); it++) 
        delete it->second;
}

