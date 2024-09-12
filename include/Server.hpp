#pragma once
/* standard includes */

#include "header.hpp"
/* default defined values */


/* Able to modify */
# define LISTENBACKLOG 20
# define MAX_BUFF_SIZE 64000 // 8kb
# define MAX_COMMAND_LENGHT 32000 // 4kb
# define MAX_CLIENTS 1000

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

	std::deque<Channel*>channels;

	std::vector<pollfd>	clientsQueue;

	std::string				port;
	int						port_number;
	std::string				password;

	char					*buff;

	std::map<int, std::string> requestsBuff;
	std::map<int, Client *> fdToClient;
	std::set<std::string> clientsNicknames;
	std::map<std::string, Client*> nameToClient; 
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
// 3500
void	checkClientsRequests(s_server_data &serverData);

/* rghouzra task - undefined */
void newCmnd(int serverSocket, Client *client,
	std::string &cmnd, s_server_data &serverData);


/*channel commands*/

/*
    * i: Set/remove Invite-only channel
    * t: Set/remove the restrictions of the TOPIC command to channel operators
    * k: Set/remove the channel key (password)
    * o: Give/take channel operator privilege
    * l: Set/remove the user limit to channel
*/
void mode(Channel *channel,  Client *client, modeopt opt, std::vector<std::string> extra_params,int _do, std::map<std::string, Client*>name_to_client, size_t size);
bool join(Client *client, Channel *chan, std::string key);
void topic(Channel *chan, Client *client, std::vector<string>params);
void kick(Client *client, Channel *chan, Client *target, \
 std::string reason, std::deque<Channel *> &channels, std::string chanName, std::string targetName);
void invite(Channel *chan, Client *client, Client *target, std::string chanName, std::string targetName);
void part(Client *client, Channel *chan, std::deque<Channel *> *channels);
/*channel utils*/
modeopt get_which_opt(std::vector<string> &cmds, int32_t size, int &plus);
