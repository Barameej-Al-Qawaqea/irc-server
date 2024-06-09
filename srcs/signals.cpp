/*
yrhiba.github.io (c). 2022-2024
*/

#include "header.hpp"

void sigpipeHandler(int signal)
{
	(void)signal;
	std::cout << "Closing the Server, Received SIGPIPE signal." << std::endl;
	exit(0);
}
