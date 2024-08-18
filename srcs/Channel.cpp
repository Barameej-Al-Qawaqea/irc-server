#include "header.hpp"


Channel::Channel(const std::string &_name):name(_name){

}

bool Channel::isChanOp(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(Chan_operators.begin(), Chan_operators.end(), client);
    return (it != Chan_operators.end());
}

bool Channel::isOnChan(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(clients.begin(), clients.end(), client);
    return (it != clients.end());
}


void Channel::AddToChan(const Client &client){
    clients.push_back(client);
    std::cout<<"client added to #" << name << '\n';
}

void Channel::removeClient(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(clients.begin(), clients.end(), client);
    if(it != clients.end()){
        clients.erase(it);
    }
}

Channel::~Channel(){
    std::cout << "Channel destructor\n";
}