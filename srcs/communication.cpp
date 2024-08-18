#include "header.hpp"

int sendMsg(int clientFd, const std::string &msg)
{
	int sendStatus = send(clientFd, msg.c_str(), msg.size(), 0);
	if (sendStatus < 0) return (-1);
	return (0);
}

std::string recvMsg(s_server_data &serverData, int clientIdx)
{
	int recvStatus = recv(serverData.clients[clientIdx].fd, serverData.buff, MAX_BUFF_SIZE, 0);
	if (recvStatus <= 0) return std::string("");
	serverData.buff[recvStatus] = '\0';
	std::string rsp = std::string(serverData.buff);
	return (rsp);
}
