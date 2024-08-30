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
        std::string password;
        std::string topic;
        std::vector<int>chan_operators;
        int limit;
        Channel();
    public:
        const std::string &getTopic();
        void setTopic(const std::string &_topic);
        const std::string &getName(){
            return name;
        }
        std::string getPassword(){
            return password;
        }
        int getlimit(){
            return this->limit;
        }
        Channel(const std::string &name);
        bool operator==(const Channel &chan) const;
        bool isChanOp(Client *client);
        bool isOnChan(const Client &client);
        void AddToChan(Client  client);
        void removeClient(const Client &client);
        const std::vector<Client> &getChanClients();
        void AddToChanOPs( Client client);
        Mode getMode(){
            return  mode;
        }
        /*mode commands*/
        void set_remove_invite_only(Client *client, bool _do){
            if(isChanOp(client)){
                std::cout << (_do ? "channel mode join changed to invite only" : "channel mode join changed to default")  << '\n';
                mode.invite_only = _do;
                return;
            }
            std::cerr << "Not permitted\n";
        }
        void set_remove_topic_restriction(Client *client, bool _do){
            if(isChanOp(client)){
                std::cout << (_do? "channel mode changed to topic restriction":"channel mode changed to default")  << '\n';
                mode.TopicRestricted = _do;
                return;
            }
            std::cerr << "Not permitted\n";
        }
        void set_remove_channel_key(Client *client, bool _do, std::string _password){
            if(isChanOp(client)){
                mode.ChanReqPass = _do;
                password = _password;
                return;
            }
            std::cerr << "set_remove_channel_key: Not permitted\n";
        }
        void add_clientToChanops(Client *client, Client *target, bool _do){
                if(isChanOp(client)){
                    if(!isChanOp(target) && _do){
                        chan_operators.push_back(target->getSocket());
                    }
                    return ;
                }
                std::cerr << "Not permitted\n";
        }
        void limitUserToChan(Client *client,bool _do, int _limit){
                if(isChanOp(client)){
                    mode.UserLimit = _do;
                    if(_do && limit >= (int)clients.size()){
                        mode.UserLimit = 1;
                        limit = _limit;
                    }
                    return ;
                }
                std::cerr << "Not permitted\n";
        }

        ~Channel();
};
#endif
