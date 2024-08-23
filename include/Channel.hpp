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
        // channel operators
        Mode mode; 
        std::string topic;
        std::vector<Client>chan_operators;
        Channel();
    public:
        const std::string &getTopic();
        const std::string &setTopic(const std::string &_topic);
        const std::string &getName(){
            return name;
        }
        Channel(const std::string &name);
        bool operator==(const Channel &chan) const;
        bool isChanOp(const Client &client);
        bool isOnChan(const Client &client);
        void AddToChan(const Client &client);
        void removeClient(const Client &client);
        const std::vector<Client> &getChanClients();
        void AddToChanOPs(const Client &client);
        Mode getMode(){
            return  mode;
        }
        /*mode commands*/
        void set_remove_invite(const Client &client, bool _do){
            if(isChanOp(client)){
                mode.invite_only = _do;
                return;
            }
            std::cerr << "Not permitted\n";
        }
        ~Channel();
};
#endif

