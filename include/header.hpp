#ifndef HEADER_HPP
# define HEADER_HPP

/* standard includes */
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include <random>
#include <sstream>
#include <csignal>

/* sockets includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

/*built-in headers*/
#include <Client.hpp>
#include <Channel.hpp>
#include <Command.hpp>


/* default defined values */


/* Able to modify */
# define LISTENBACKLOG 20
# define MAX_BUFF_SIZE 64000 // 8kb
# define MAX_COMMAND_LENGHT 32000 // 4kb
# define MAX_CLIENTS 100

/* constants */
# define ON 1
# define OFF 0

class Client; 
struct s_server_data
{
	int					sockfd;
	int					bindStatus;
	int					listenStatus;
	int					sendStatus;
	int					serverStatus;
	int					pollStatus;

	struct sockaddr_in	serverAddress;

	std::vector<pollfd>	clients;

	std::vector<pollfd>	clientsQueue;

	std::string				port;
	int						port_number;
	std::string				password;

	char					*buff;

	std::map<int, std::string> requestsBuff;
	std::map<int, Client *> fdToClient;
	std::set<std::string> clientsNicknames;
	s_server_data()
	{
		sockfd = 0;
		bindStatus = 0;
		listenStatus = 0;
		sendStatus = 0;
		serverStatus = ON;
		clients = std::vector<pollfd>();
		buff = new char[MAX_BUFF_SIZE + 1];
	}

	~s_server_data()
	{
		delete [] buff;
	}
};

/* sockets */
void	createSocket(s_server_data &serverData);
void	setupServerSocket(s_server_data &serverData);
int		setSocketAsNonBlocking(int sockfd);
void	serverSocketListen(s_server_data &serverData);
void	closeSocket(int sockfd);
std::pair<int, Client*> acceptNewConnection(s_server_data &serverData);

/* communication */
int			sendMsg(int clientFd, const std::string &msg);
std::string	recvMsg(s_server_data &serverData, int clientIdx);

/* signals */
void	sigpipeHandler(int signal);

/* utils */
pollfd	newPollFd(int sockfd);
int		isValidPort(std::string &port);
int		addRequest(s_server_data &serverData, int clientIdx, std::string &responce);

/* parsing */
void	parsingAndSetup(s_server_data &serverData, int ac, char **av);
void	serverSetup(s_server_data &serverData);

/* steps */
// 1
void socketsPolling(s_server_data &serverData);
// 2
void	checkNewClientAttempt(s_server_data &serverData);
// 3
void	checkClientsRequests(s_server_data &serverData);

/* rghouzra task - undefined */
void newCmnd(int serverSocket, Client *client,
	std::string &cmnd, s_server_data &serverData);


#endif
