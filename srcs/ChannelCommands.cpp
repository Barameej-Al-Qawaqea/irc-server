

#include "header.hpp"

static const char *CHAN_NAME_ = "&#";


/* chanops commands */
// KICK    - Eject a client from the channel
// MODE    - Change the channel's mode
// INVITE  - Invite a client to an invite-only channel (mode +i)
// TOPIC   - Change the channel topic in a mode +t channel

void join(Client *client, Channel &chan){
    if(chan.isOnChan(*client) || chan.getMode().invite_only){
        // err;
        return;
    }
    chan.AddToChan(*client);
    client->setcurrChan(&chan);
}

void mode(Channel *channel, const Client *client, modeopt opt, std::vector<std::string> extra_params,int _do){
    std::vector<std::string>::iterator params = extra_params.begin();
    if(!channel|| !channel->isChanOp(*client)){
        std::cerr << "Invalid operation\n";
        return;
    }
    params+=2;
    switch(opt){
        case INVITE_ONLY_OPT:
            ;
        case TOPIC_RESTRICTION_OPT:
            ;
        case CHAN_KEY_OPT:
            ;
        case CHANOP_OPT:
            ;
        case USER_LIMIT_OPT:
            ;
        case UNKOWN:
            std::cerr << "UNKOWN OPTION\n";
    }
    
}

void kick(Client &client, Channel &chan, Client &target){
    
    if(!chan.isOnChan(client)){
        // err;
        return;
    }
    
    if(!chan.isChanOp(client)){
        // err;
        return;
    }
    if(!chan.isOnChan(target)){
        // err;
        return;
    }
    chan.removeClient(client);
}


void topic(Channel &chan, const Client &client){
    
}