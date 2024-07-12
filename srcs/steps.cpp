# include "header.hpp"
# include "Client.hpp"

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

			if (sendMsg(newClient.first, "Welcome to ft_irc server.\r\n1337@2024 createad by [yrhiba, rghouzra].\r\n") < 0)
			{
				// std::cout << "failed sending message to " << serverData.clients.back().fd << std::endl;
				return ;
			}

			serverData.clients.push_back(newPollFd(newClient.first));
			serverData.fdToClient[newClient.first] = newClient.second;
		}
		else
		{
			// std::cout << "faild accpeting new connection." << std::endl;
		}
	}
}

void removeClient(s_server_data &serverData, int clientIdx)
{
	closeSocket(serverData.clients[clientIdx].fd);
	std::swap(serverData.clients[clientIdx], serverData.clients[serverData.clients.size() - 1]);
	serverData.clients.pop_back();
	serverData.fdToClient.erase(clientIdx);
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
					std::string cmnd = serverData.requestsBuff[clientIdx];
					serverData.requestsBuff.erase(clientIdx);
					cmnd.pop_back(); // remove the '\n'
					newCmnd(serverData.sockfd, serverData.fdToClient[clientIdx], cmnd, serverData);
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
