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
        // channel operators
        std::string topic;
        std::vector<Client>chan_operators;
        Channel();
    public:
        const std::string &getTopic();
        const std::string &setTopic(const std::string &_topic);
        Channel(const std::string &name);
        bool operator==(const Channel &chan) const;
        bool isChanOp(const Client &client);
        bool isOnChan(const Client &client);
        void AddToChan(const Client &client);
        void removeClient(const Client &client);
        const std::vector<Client> &getChanClients();
        void AddToChanOPs(const Client &client);
        ~Channel();
};
#endif

