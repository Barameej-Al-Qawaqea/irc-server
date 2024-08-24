

#include "header.hpp"

// static const char *CHAN_NAME_ = "&#";


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
            channel->set_remove_topic_restriction(*client, _do);
            break;
        case CHAN_KEY_OPT:
            channel->set_remove_channel_key(*client, _do, *params);
            break;
        case CHANOP_OPT:
            // channel->add_clientToChanops(*client, , _do);
            // need achraf help on this
            break;
        case USER_LIMIT_OPT:
            channel->limitUserToChan(*client, _do, std::atoi(params->c_str()));
            break;
        case UNKOWN:
            std::cerr << "UNKOWN OPTION\n";
            break;
    }
    
}

void kick(Client &client, Channel &chan, Client &target){
    
    if(!chan.isChanOp(client)){
        // err;
        std::cerr << client.getNickName() << " is not with chanops\n";
        return;
    }
    if(!chan.isOnChan(target)){
        // err;
        std::cerr << target.getNickName() << "is not in in the " << chan.getName() << '\n';
        return;
    }
    chan.removeClient(client);
}


void topic(Channel *chan, const Client *client, std::string topic, int _do){
    if(chan->getMode().TopicRestricted && !chan->isChanOp(*client)){
        std::cerr << "Not permitted\n";
        return;
    }
    if(_do){
        chan->setTopic(topic);
        std::cout << "Topic changed to " << topic << '\n';
    }
    else{
        std::cout << chan->getTopic() << '\n';
    }

}