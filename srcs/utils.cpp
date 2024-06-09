/*
yrhiba.github.io (c). 2022-2024
*/

#include "header.hpp"

pollfd newPollFd(int sockfd)
{
	pollfd poll;
	poll.fd = sockfd;
	poll.events = POLLIN;
	poll.revents = 0;
	return (poll);
}

int isValidPort(std::string &port)
{
	int number = 0;
	for (int i = 0; i < int(port.size()); i++)
	{
		if (!std::isdigit(port[i])) return (-1);
		number = number * 10 + (port[i] - '0');
		if (number > 65535) return (-1);
	}
	return (number);
}

int addRequest(s_server_data &serverData, int clientIdx, std::string &responce)
{
	if (!(serverData.requestsBuff[serverData.clients[clientIdx].fd].empty()))
	{
		serverData.requestsBuff[serverData.clients[clientIdx].fd] += " ";
	}

	serverData.requestsBuff[serverData.clients[clientIdx].fd] += responce;

	if (serverData.requestsBuff[serverData.clients[clientIdx].fd].size() > MAX_COMMAND_LENGHT)
	{
		// response go to big. !!! ignore it;
		serverData.requestsBuff.erase(serverData.clients[clientIdx].fd);
		return (-1);
	}

	return (responce.find('\n') != std::string::npos);
}
