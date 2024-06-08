/*
yrhiba.github.io (c). 2022-2024
*/

#include "header.hpp"

/* sockets methodes */
void createSocket(s_server_data &serverData)
{
	serverData.sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverData.sockfd < 0)
	{
		std::cout << "createSocket()::creating socket Err" << std::endl;
		exit(0);
	}
}

void setupServerSocket(s_server_data &serverData)
{
	serverData.serverAddress.sin_family = AF_INET;
	serverData.serverAddress.sin_port = htons(serverData.port_number);  // Convert to network byte order
	serverData.serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface

	serverData.bindStatus = bind (
				serverData.sockfd,
				(struct sockaddr *)&(serverData.serverAddress),
				sizeof(serverData.serverAddress)
			);

	if (serverData.bindStatus < 0) 
	{
		std::cout << "server:: failed binding." << std::endl;
		exit(0);
	}
}

int setSocketAsNonBlocking(int sockfd)
{
	int status = fcntl(sockfd, F_SETFL, O_NONBLOCK);
	if (status < 0) return (-1);
	return (0);
}

void serverSocketListen(s_server_data &serverData)
{
	serverData.listenStatus = listen(serverData.sockfd, LISTENBACKLOG);
	if (serverData.listenStatus < 0)
	{
		std::cout << "serverSocketListen()::setting listen backlog Err" << std::endl;
		exit(0);
	}
}

void closeSocket(int sockfd)
{
	close(sockfd);
}

std::pair<int, sockaddr_in> accpetNewConnection(s_server_data &serverData)
{
	sockaddr_in clientAddr {};
	socklen_t size = sizeof(clientAddr);
	int clienSocket = accept(serverData.sockfd, (sockaddr *)&(clientAddr), &size);
	return (std::make_pair(clienSocket, clientAddr));
}
