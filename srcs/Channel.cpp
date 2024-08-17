#include "header.hpp"


Channel::Channel(const std::string &_name):name(_name){

}

bool Channel::isChanOp(const Client &client){
    std::vector<Client>::iterator it;

    it = std::find(Chan_operators.begin(), Chan_operators.end(), client);
    return (it != Chan_operators.end());
}

Channel::~Channel(){
    std::cout << "Channel destructor\n";
}