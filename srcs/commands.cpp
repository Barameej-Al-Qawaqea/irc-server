#include "header.hpp"



void    Command::executeJoin(){}
void    Command::executeInvite(){;}
void    Command::executeTopic(){;}

void    Command::executeMode(){
    int plus;
    mode(this->client->getcurrChan(), this->client, get_which_opt(cmd, ((cmd.size() < 3) * -1), plus), cmd , plus);
}

void    Command::executeKick(){;}
