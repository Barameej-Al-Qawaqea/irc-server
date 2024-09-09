#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Client.hpp"

#define MAX_CHAN_NAME_LEN 200
#define CHAN_NAME_RSTD_CHARS " ,"

using std::vector;
using std::string;

typedef enum t_modeopt{
    INVITE_ONLY_OPT,
    TOPIC_RESTRICTION_OPT,
    CHAN_KEY_OPT,
    CHANOP_OPT,
    USER_LIMIT_OPT,
    UNKOWN
}modeopt;

typedef struct t_mode{
    int invite_only;
    int TopicRestricted;
    int ChanReqPass;
    long long UserLimit;
}Mode;

class Channel{
    private :
        std::string name;
        std::vector<Client>clients;
        std::deque<Client>invited_clients;
        // channel operators
        Mode mode; 
        std::string password;
        std::string topic;
        std::vector<Client>chan_operators;
        int limit;
        Channel();
    public:
        const std::string &getTopic();
        void setTopic(const std::string &_topic);
        std::string getModeString();
        const std::string &getName();
        std::string getPassword();
        void addPendingClient(Client client);
        void removePendingClient(Client client);
        std::deque<Client> &getPendingClients();
        bool isPendingClient(Client client);
        int getlimit();
        Channel(const std::string &name);
        bool operator==(const Channel &chan) const;
        bool isChanOp(Client *client);
        bool isOnChan(Client *client);
        void AddToChan(Client  client);
        void removeClient(Client client);
        void removeChanop(Client client);
        const std::vector<Client> &getChanClients();
        void AddToChanOPs( Client client);
        void debug();
        Mode getMode();
        /*mode commands*/
        void set_remove_invite_only(Client *client, bool _do);

        void set_remove_topic_restriction(Client *client, bool _do);

        void set_remove_channel_key(Client *client, bool _do, std::string _password);

        void add_clientToChanops(Client *client, Client *target, bool _do);

        void limitUserToChan(Client *client,bool _do, int _limit);
        ~Channel();
};
#endif
