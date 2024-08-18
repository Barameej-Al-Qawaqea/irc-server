#include "header.hpp"

static const char *CHAN_NAME_ = "&#";


/* chanops commands */
// KICK    - Eject a client from the channel
// MODE    - Change the channel's mode
// INVITE  - Invite a client to an invite-only channel (mode +i)
// TOPIC   - Change the channel topic in a mode +t channel

void join(Client &client, Channel &chan){
    if(chan.isOnChan(client)){
        // err;
        return;
    }
    chan.AddToChan(client);
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
