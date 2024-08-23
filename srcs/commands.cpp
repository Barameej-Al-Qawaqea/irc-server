#include "header.hpp"



void    Command::executeJoin(){
    std::string name = cmd[1];
    if(cmd.size() != 2 || cmd[1].length() < 1||(cmd[1][0] != '#' && cmd[1][0] != '&')){
        std::cerr << "invalid join params\n"; 
        return ;
    }
    // todo: skip # in the first name of the channel
    name = std::string(cmd[1].c_str() );
    Channel *chan = new Channel(name);
    if(join(this->client, *chan)){
        serverData.channels.push_back(*chan);
    }
}
void    Command::executeInvite(){;}
void    Command::executeTopic(){;}

void    Command::executeMode(){
    int plus;
    mode(this->client->getcurrChan(), this->client, get_which_opt(cmd, ((cmd.size() < 3) * -1), plus), cmd , plus);
}

void    Command::executeKick(){;}
