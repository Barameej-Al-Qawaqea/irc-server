# include "header.hpp"

void socketsPolling(s_server_data &serverData)
{
	serverData.pollStatus = poll(&serverData.clients[0], serverData.clients.size(), -1);
	if (serverData.pollStatus == -1)
	{
		std::cerr << "poll return (-1)" << std::endl;
		exit (0);
	}
}

void	checkNewClientAttempt(s_server_data &serverData)
{
	if (serverData.clients[0].revents & POLLIN)
	{
		std::pair<int, Client*> newClient = acceptNewConnection(serverData);

		if (newClient.first >= 0 && serverData.clients.size() < MAX_CLIENTS)
		{
			// if (sendMsg(newClient.first, ":server :Welcome to ft_irc server createad by [asettar, yrhiba, dexter].\r\n") < 0)
			// {
			// 	// std::cout << "failed sending message to " << serverData.clients.back().fd << std::endl;
			// 	return ;
			// }

			serverData.clients.push_back(newPollFd(newClient.first));
			serverData.fdToClient[newClient.first] = newClient.second;
		}
		else
		{
			// std::cout << "faild accpeting new connection." << std::endl;
		}
	}
}

void clean_chan_data(s_server_data &serverData, Client *client)
{
	std::deque<Channel*> &channels = serverData.channels;
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *chan = channels[i];
		if (chan->isOnChan(client))
		{
			chan->removeClient(client);
			if (chan->getChanClients().empty() && chan->getName() != "general")
			{
				if(chan->isChanOp(client))
					chan->removeChanop(client);
				std::swap(channels[i], channels[channels.size() - 1]);
				channels.pop_back();
				delete chan;
				i -= 1;
			}
		}
	}
}

void	updateServerData(s_server_data &serverData, Client *client) {
	if (serverData.clientsNicknames.count(client->getNickName()))
		serverData.clientsNicknames.erase(client->getNickName());
	if (serverData.nameToClient.count(client->getNickName()))
		serverData.nameToClient.erase(client->getNickName());
    std::set<int> &activeChatsSockets = client->getActiveChatsSockets();
    for(std::set<int>::iterator it = activeChatsSockets.begin(); it != activeChatsSockets.end(); it++) {
		int userFd = *it;
		if (userFd == client->getSocket()) continue;
		Client *otherClient = serverData.fdToClient[userFd];
		otherClient->deleteActiveChat(client->getSocket());
	}
}

void removeClient(s_server_data &serverData, int clientIdx)
{
	closeSocket(serverData.clients[clientIdx].fd);
	Client *client = serverData.fdToClient[serverData.clients[clientIdx].fd];


	clean_chan_data(serverData, client);
	updateServerData(serverData, client);
	std::swap(serverData.clients[clientIdx], serverData.clients[serverData.clients.size() - 1]);
	serverData.fdToClient.erase(serverData.fdToClient.find(serverData.clients.back().fd));
	serverData.clients.pop_back();
	delete client;
}

void	checkClientsRequests(s_server_data &serverData)
{
	for (int i = 1; i < int(serverData.clients.size()); i++)
		if (serverData.clients[i].revents & POLLIN)
		{
			std::string response = recvMsg(serverData, i);
			//
			if (response.empty())
			{
				removeClient(serverData, i);
				i -= 1;
			}
			else
			{
				int r = addRequest(serverData, i, response);
				if (r > 0)
				{
					int clientIdx = serverData.clients[i].fd;
					std::string &cmnd = serverData.requestsBuff[clientIdx];
					std::stringstream ss(cmnd);
					std::string sep = "\n";

					if (cmnd.size() > 1 && cmnd[cmnd.size() - 2] == '\r') sep = "\r\n";   // not from terminal
					size_t end, start = 0;
					while ((end = cmnd.find(sep, start)) != std::string::npos) {
						std::string toSend = cmnd.substr(start, end - start);
						newCmnd(serverData.sockfd, serverData.fdToClient[clientIdx], toSend, serverData);
						start = end + sep.size();
					}
					serverData.requestsBuff.erase(clientIdx);
				}
				else if (r < 0)
				{
					// response to big!.
					// do i should send an err responce to the client or remove the client ?
					removeClient(serverData, i);
				}
			}
		}
}
