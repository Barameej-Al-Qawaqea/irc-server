/*
yrhiba.github.io (c). 2022-2024
*/
# include "header.hpp"

void newCmnd(int serverSocket, int clientSocket, sockaddr_in &clientAddr, std::string &cmnd, s_server_data &serverData)
{
	if(CorrectPass(cmnd, serverData.password)){
		
	}
}

/* main funciton */
int main(int ac, char **av)
{
	s_server_data serverData;
	std::signal(SIGPIPE, sigpipeHandler);
	parsingAndSetup(serverData, ac, av);
	serverSetup(serverData);

	while (serverData.serverStatus == ON)
	{
		/* for debugging + testing code bugs */
		assert(serverData.clients.size() == serverData.clientsAddr.size() && "yap!, code have bug");
		/* step 1 : polling */
		socketsPolling(serverData);
		/* step 2 : checking new connection attempt */
		checkNewClientAttempt(serverData);
		/* step 3 : check clients sockets requests */
		checkClientsRequests(serverData);
	}

	/* close sockets */
	for (int i = 0; i < int(serverData.clients.size()); i++)
		closeSocket(serverData.clients[i].fd);

	return (0);
}
