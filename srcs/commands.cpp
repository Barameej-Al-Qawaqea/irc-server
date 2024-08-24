#include "header.hpp"


Channel *findChan(std::string name, std::deque<Channel> channels, bool &created){
    std::deque<Channel>::iterator it;

    it = find(channels.begin(), channels.end(), name);
    if(it == channels.end()){
        created = true;
        return new Channel(name);
    }
    created = false;
    return &(*it);
}

void    Command::executeJoin(){
    std::string name = cmd[1];
    bool created  = false;
    if(cmd.size() != 2 || cmd[1].length() < 2||(cmd[1][0] != '#' && cmd[1][0] != '&')){
        std::cerr << "invalid join params\n"; 
        return ;
    }
    // todo: skip # in the first name of the channel
    name = std::string(cmd[1].c_str() + 1);
    
    Channel *chan = findChan(name, this->serverData.channels, created);
    if(join(this->client, *chan) && created){
        serverData.channels.push_back(*chan);
    }
}

void    Command::executeInvite(){;}
void    Command::executeTopic(){
    if(cmd.size()==1){
        topic(this->client->getcurrChan(), this->client,"", 0 );
    }
    else{
        topic(this->client->getcurrChan(), this->client, cmd[1], 1);
    }
}

void    Command::executeMode(){
    int plus;
    mode(this->client->getcurrChan(), this->client, get_which_opt(cmd, ((cmd.size() < 3) * -1), plus), cmd , plus);
}

void    Command::executeKick(){;}
