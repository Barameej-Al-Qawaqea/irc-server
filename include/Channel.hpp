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
        const std::string &getName(){
            return name;
        }
        std::string getPassword(){
            return password;
        }
        void addPendingClient(Client client){
            invited_clients.push_back(client);
        }
        void removePendingClient(Client client){
            std::deque<Client>::iterator it;
            it = std::find(invited_clients.begin(), invited_clients.end(), client);
            if(it != invited_clients.end()){
                invited_clients.erase(it);
            }
        }

        std::deque<Client> &getPendingClients(){
            return invited_clients;
        }

        bool isPendingClient(Client client){
            std::deque<Client>::iterator it;
            it = std::find(invited_clients.begin(), invited_clients.end(), client);
            return (it != invited_clients.end());
        }
        int getlimit(){
            return this->limit;
        }
        Channel(const std::string &name);
        bool operator==(const Channel &chan) const;
        bool isChanOp(Client *client);
        bool isOnChan(Client *client);
        void AddToChan(Client  client);
        void removeClient(Client client);
        void removeChanop(Client client);
        const std::vector<Client> &getChanClients();
        void AddToChanOPs( Client client);
        void debug(){
            std::cout << "channel members : {\n";
            for(size_t i = 0; i < clients.size(); i++){
                std::cout << clients[i].getNickName() << '\n';
            }
            std::cout << "}\n";
            std::cout << "channel operators : {\n";
            for(size_t i = 0; i < chan_operators.size(); i++){
                std::cout << chan_operators[i].getNickName() << '\n';
            }
            std::cout << "}\n";
            std::cout << "channel pending clients : {\n";
            for(size_t i = 0; i < invited_clients.size(); i++){
                std::cout << invited_clients[i].getNickName() << '\n';
            }
            std::cout << "}\n";
            std::cout << "channel mode : {\n";
            std::cout << "invite_only : " << mode.invite_only << '\n';
            std::cout << "TopicRestricted : " << mode.TopicRestricted << '\n';
            std::cout << "ChanReqPass : " << mode.ChanReqPass << '\n';
            std::cout << "UserLimit : " << mode.UserLimit << '\n';
            std::cout << "}\n";
            std::cout << "channel topic : " << topic << '\n';
            fflush(stdout);
        }
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
                        chan_operators.push_back(*target);
                    }
                    else if(isChanOp(target) && !_do){
                        // 
                        std::vector<Client>::iterator it;
                        it = std::find(chan_operators.begin(), chan_operators.end(), *target);
                        if(it != chan_operators.end()){
                            chan_operators.erase(it);
                        }
                    }
                    return ;
                }
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
