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
			if (setSocketAsNonBlocking(newClient.first) < 0)
			{
				// failed estabilsh connection : can't set socket to non-blocking i/o.
				return ;
			}

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
			chan->removeClient(*client);
			if (chan->getChanClients().empty())
			{
				if(chan->isChanOp(client))
					chan->removeChanop(*client);
				delete chan;
				std::swap(channels[i], channels[channels.size() - 1]);
				channels.pop_back();
				i -= 1;
			}
		}
	}
}

void removeClient(s_server_data &serverData, int clientIdx)
{
	closeSocket(serverData.clients[clientIdx].fd);
	Client *client = serverData.fdToClient[serverData.clients[clientIdx].fd];
	if (!client) std::cout << "yes, isNULL\n";
	if (serverData.clientsNicknames.count(client->getNickName()))
		serverData.clientsNicknames.erase(client->getNickName());
	if (serverData.nameToClient.count(client->getNickName()))
		serverData.nameToClient.erase(client->getNickName());
	std::swap(serverData.clients[clientIdx], serverData.clients[serverData.clients.size() - 1]);
	serverData.fdToClient.erase(serverData.fdToClient.find(serverData.clients.back().fd));
	serverData.clients.pop_back();
	clean_chan_data(serverData, client);
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
					// serverData.requestsBuff.erase(clientIdx);
					// while (cmnd.size() && (cmnd.back() == '\n' || cmnd.back() == '\r'))
					// 	cmnd.pop_back(); // remove the '\n'
					//p\n
					// p\r\n
					int idx = cmnd.find("\n");
					// std::cout << "all:" << cmnd << std::endl;
					// std::cout << "commandsize: " << cmnd.size() << std::endl;
					bool isLimeChat = 0;
					if (idx > 0 && cmnd[idx - 1] == '\r') isLimeChat = 1;
					std::string toSend = cmnd.substr(0, idx - isLimeChat);
					// std::cerr << "command: " << cmnd << std::endl;
					cmnd.erase(cmnd.begin(), cmnd.begin() + idx + 1);
					// std::cerr << "command: " << cmnd << std::endl;
					// std::cerr << "::: " << toSend << " ||| " << cmnd << std::endl;
					if (cmnd.size() && (cmnd.back() == '\n' || cmnd.back() == '\r')) std::cout << "Yesis \\r\n";
					// std::cout << "new size: " << cmnd.size() << '\n';
					newCmnd(serverData.sockfd, serverData.fdToClient[clientIdx], toSend, serverData);
				}
				// b\n
				else if (r < 0)
				{
					// response to big!.
					// do i should send an err responce to the client or remove the client ?
					removeClient(serverData, i);
				}
			}
		}
}
