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

int main(int ac, char **av) {
    int serverSocket = establichServerConnection();
    char *buff = new char[MAX_BUFF_SIZE + 1];
    while (1) {
        int bytesRead = recv(serverSocket, buff, MAX_BUFF_SIZE, 0);
        if (bytesRead < 0) {
            std::cout << "An error occurs while reading server replies\n";
            exit(0);
        }
        // if (send)
    }
}  
