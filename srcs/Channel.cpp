#include "header.hpp"


Channel::Channel(const std::string &_name):name(_name){
    topic = "undefined";
    mode.ChanReqPass = 0;
    mode.invite_only = 0;
    mode.TopicRestricted = 0;
    mode.UserLimit = 0;
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
    it = std::find(chan_operators.begin(), chan_operators.end(), client);
    return (it != chan_operators.end());
}

bool Channel::isOnChan(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(clients.begin(), clients.end(), client);
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

void Channel::removeClient(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(clients.begin(), clients.end(), client);
    if(it != clients.end()){
        clients.erase(it);
    }
}

void Channel::removeChanop(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(chan_operators.begin(), chan_operators.end(), client);
    if(it != chan_operators.end()){
        chan_operators.erase(it);
    }
}
Channel::~Channel(){
    
}