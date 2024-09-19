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

std::vector<std::string>SplitString(const std::string &string, char delim){
	std::istringstream Sstream(string);
	std::vector<std::string>arr;
	std::string buffer;
	while (getline(Sstream, buffer, delim))
	{
		std::cout << buffer << '\n';
	}
	return arr;
}

bool CorrectPass(const std::string &UserCmnd, const std::string &ServerPassword){
	(void)UserCmnd;
	(void)ServerPassword;
    return true;
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


//channel utils

modeopt invalid_cmd(std::string error){
	std::cerr << error << '\n';
	return UNKOWN;
}


modeopt get_which_opt(std::vector<string> &cmds, int32_t size, int &plus){
	std::string chan, opt;
	std::string available_opt = "itkol";
	if(size == -1)
		return invalid_cmd("NOT ENOUG ARGUMENTS");
	chan = cmds[1];
	if(chan[0] != '#')
		return invalid_cmd("MISLEADING CHAN NAME");
	opt = cmds[2];
	plus = (opt[0] == '+' ? 1 : (opt[0] == '-' ? 0 : -1));
	if(opt.size() < 2 || plus < 0)
		return UNKOWN;
	for(size_t i = 0; i < available_opt.size(); i++){
		if(available_opt[i] == opt[1])
			return (modeopt)i;
	}
	return invalid_cmd("UNKOWN OPTION");
}
