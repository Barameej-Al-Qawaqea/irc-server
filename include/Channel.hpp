#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Client.hpp"

#define MAX_CHAN_NAME_LEN 200
#define CHAN_NAME_RSTD_CHARS " ,"

using std::vector;
using std::string;


class Channel{
    private :
        std::string name;
        std::vector<Client>clients;
        std::vector<Client>Chan_operators;
        Channel();
    public:
        Channel(const std::string &name);
        bool isChanOp(const Client &client);
        bool isOnChan(const Client &client);
        void AddToChan(const Client &client);
        ~Channel();
};
#endif