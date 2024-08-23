

#include "header.hpp"

static const char *CHAN_NAME_ = "&#";


/* chanops commands */
// KICK    - Eject a client from the channel
// MODE    - Change the channel's mode
// INVITE  - Invite a client to an invite-only channel (mode +i)
// TOPIC   - Change the channel topic in a mode +t channel

bool join(Client *client, Channel &chan){
    if(chan.isOnChan(*client) || chan.getMode().invite_only){
        std::cout << client->getNickName() << " cant join #" << chan.getName() << '\n';
        return false;
    }
    chan.AddToChan(*client);
    client->setcurrChan(&chan);
    std::cout << client->getNickName() << " joined #" << chan.getName() << '\n';
    return true;
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
            channel->set_remove_invite_only(*client, _do);
            break;
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