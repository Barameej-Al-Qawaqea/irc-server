#include "header.hpp"


Channel::Channel(const std::string &_name):name(_name){
    topic = "undefined";
    mode.ChanReqPass = 0;
    mode.invite_only = 0;
    mode.TopicRestricted = 0;
    mode.UserLimit = -1;
}

bool Channel::operator==(const Channel &chan)const{
    return (chan.name == this->name);
}

bool isChanExit(Channel &chan, vector<Channel> &channels){
    vector<Channel>::iterator it;

    it = std::find(channels.begin(), channels.end(), chan);
    return (it != channels.end());
}

bool Channel::isChanOp(const Client &client){
    std::vector<Client>::iterator it;

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

const std::string &Channel::setTopic(const std::string &_topic){
    topic = _topic;
}
void Channel::AddToChan(const Client &client){
    clients.push_back(client);

}

void Channel::AddToChanOPs(const Client &client){
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

Channel::~Channel(){
    std::cout << "Channel object destructor\n";
}