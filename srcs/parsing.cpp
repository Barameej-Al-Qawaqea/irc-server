# include "header.hpp"

void parsingAndSetup(s_server_data &serverData, int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "err:: bad arguments'." << std::endl;
		exit(0);
	}
	serverData.port = av[1];
	serverData.password = av[2];
	bool badPassword = (serverData.password.size() == 0);
	for(size_t i = 0; i < serverData.password.size(); i++)
		badPassword |= (serverData.password[i] == ' ');
	if (badPassword || serverData.password.empty()) {
		std::cout << "Invalid Password\n";
		exit(0);
	}
	serverData.port_number = isValidPort(serverData.port);
	if (serverData.port_number < 0 || serverData.port.empty())
	{
		std::cout << "err:: invalid port'." << std::endl;
		exit(0);
	}
}

void	serverSetup(s_server_data &serverData)
{
	createSocket(serverData);
	int opt = 1;
	if (setsockopt(serverData.sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt failed");
		close(serverData.sockfd);
		exit(0);
	}
	if (setSocketAsNonBlocking(serverData.sockfd) < 0)
	{
		perror("setsockopt failed");
		close(serverData.sockfd);
		exit(0);
	}
	setupServerSocket(serverData);
	serverSocketListen(serverData);
	serverData.clients.push_back(newPollFd(serverData.sockfd));
}
