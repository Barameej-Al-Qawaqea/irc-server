#include "header.hpp"


Channel::Channel(const std::string &_name):name(_name){
    topic = "";
    mode.ChanReqPass = 0;
    mode.invite_only = 0;
    mode.TopicRestricted = 0;
    mode.UserLimit = 0;
}

const std::string &Channel::getName(){
            return name;
}

std::deque<Client> &Channel::getPendingClients(){
            return invited_clients;
}
void Channel::removePendingClient(Client client){
            std::deque<Client>::iterator it;
            it = std::find(invited_clients.begin(), invited_clients.end(), client);
            if(it != invited_clients.end()){
                invited_clients.erase(it);
            }
}

bool Channel::isPendingClient(Client client){
            std::deque<Client>::iterator it;
            it = std::find(invited_clients.begin(), invited_clients.end(), client);
            return (it != invited_clients.end());
}

int Channel::getlimit(){
            return this->limit;
}

void Channel::addPendingClient(Client client){
            invited_clients.push_back(client);
}

std::string Channel::getPassword(){
            return password;
}

std::string Channel::getModeString(){
    std::string modeString = "+";
    if(mode.ChanReqPass)
        modeString += "k";
    if(mode.invite_only)
        modeString += "i";
    if(mode.TopicRestricted)
        modeString += "t";
    if(mode.UserLimit)
        modeString += "l";
    if(modeString == "+")
        modeString = "";
    return modeString;
}

bool Channel::operator==(const Channel &chan)const{
    return (chan.name == this->name);
}

bool isChanExit(Channel &chan, vector<Channel> &channels){
    vector<Channel>::iterator it;

    it = std::find(channels.begin(), channels.end(), chan);
    return (it != channels.end());
}

bool Channel::isChanOp(Client *client){
    std::vector<Client>::iterator it = chan_operators.begin();
    it = std::find(chan_operators.begin(), chan_operators.end(), *client);
    return (it != chan_operators.end());
}

bool Channel::isOnChan(Client *client){
    std::vector<Client>::iterator it;

    it = std::find(clients.begin(), clients.end(), *client);
    return (it != clients.end());
}

const std::string &Channel::getTopic(){
    return this->topic;
}

void Channel::setTopic(const std::string &_topic){
    topic = _topic;
}
void Channel::AddToChan(Client client){
    if((!mode.UserLimit) || (mode.UserLimit && (int)this->clients.size() < this->getlimit())){
        clients.push_back(client);
        if (clients.size() == 1)
            chan_operators.push_back(client);
    }
}

void Channel::AddToChanOPs(Client client){
    chan_operators.push_back(client);
}

const std::vector<Client> &Channel::getChanClients(){
    return clients;
}

void Channel::removeClient(Client client){
    std::vector<Client>::iterator it;

    it = std::find(clients.begin(), clients.end(), client);
    if(it != clients.end()){
        clients.erase(it);
    }
}

void Channel::removeChanop(Client client){
    std::vector<Client>::iterator it;

    it = std::find(chan_operators.begin(), chan_operators.end(), client);
    if(it != chan_operators.end()){
        chan_operators.erase(it);
    }
}

void Channel::set_remove_invite_only(Client *client, bool _do){
            if(isChanOp(client)){
                std::cout << (_do ? "channel mode join changed to invite only" : "channel mode join changed to default")  << '\n';
                mode.invite_only = _do;
                return;
            }
            std::cerr << "Not permitted\n";
}

void Channel::add_clientToChanops(Client *client, Client *target, bool _do){
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

void Channel::limitUserToChan(Client *client,bool _do, int _limit){
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

void Channel::set_remove_channel_key(Client *client, bool _do, std::string _password){
            if(isChanOp(client)){
                mode.ChanReqPass = _do;
                password = _password;
                return;
            }
            std::cerr << "set_remove_channel_key: Not permitted\n";
}
void Channel::set_remove_topic_restriction(Client *client, bool _do){
            if(isChanOp(client)){
                std::cout << (_do? "channel mode changed to topic restriction":"channel mode changed to default")  << '\n';
                mode.TopicRestricted = _do;
                return;
            }
            std::cerr << "Not permitted\n";
}

Mode Channel::getMode(){
            return  mode;
}

Channel::~Channel(){
    
}



void Channel::debug(){
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