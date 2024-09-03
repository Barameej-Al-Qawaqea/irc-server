

#include "header.hpp"

// static const char *CHAN_NAME_ = "&#";


/* chanops commands */
// KICK    - Eject a client from the channel
// MODE    - Change the channel's mode
// INVITE  - Invite a client to an invite-only channel (mode +i)
// TOPIC   - Change the channel topic in a mode +t channel

bool join(Client *client, Channel *chan, std::string key){
    if(chan->isOnChan(*client) || (chan->getMode().invite_only && !chan->isPendingClient(*client) )|| (chan->getMode().ChanReqPass && (chan->getPassword() != key))){
        std::cerr << "Not permitted\n";
        return false;
    }
    std::cout << "Joining " << chan->getName() << '\n';
    chan->AddToChan(*client);
    client->setcurrChan(chan);
    sendMsg(client->getSocket(), "JOIN " + chan->getName() + "\n");
    return true;
}

void    mode(Channel *channel, Client *client, modeopt opt, std::vector<std::string> extra_params, \
                int _do, std::map<std::string, Client*>name_to_client) {
    
    std::vector<std::string>::iterator params = extra_params.begin();
    Client *clientTarget;

    if(!channel|| !channel->isChanOp(client)){
        if(!channel)
            std::cerr << "Invalid operation\n";
        else
            std::cerr << "Not permitted\n";
        return;
    }
    params+=2;
    switch(opt){
        case INVITE_ONLY_OPT:
            if(*params != "+i" && *params != "-i"){
                //error
                return;
            }
            channel->set_remove_invite_only(client, _do);
            break;
        case TOPIC_RESTRICTION_OPT:
            channel->set_remove_topic_restriction(client, _do);
            break;
        case CHAN_KEY_OPT:
             if(params->size() < 2){
                //error
                return;
            }
            params++;
            channel->set_remove_channel_key(client, _do, *params);
            break;
        case CHANOP_OPT:
            if(params->size() < 2){
                //error
                return;
            }
            params++;
            if(name_to_client.find(*params) == name_to_client.end())
                return ;
            clientTarget = name_to_client[*params];
            channel->add_clientToChanops(client, clientTarget, _do);
            break;
        case USER_LIMIT_OPT:
            if(params->size() < 2){
                //error
                return;
            }
            params++;
            channel->limitUserToChan(client, _do, std::atoi(params->c_str()));
            break;
        case UNKOWN:
            std::cerr << "UNKOWN OPTION\n";
            break;
    }
    
}

void kick(Client *client, Channel *chan, Client *target){
    if(!chan)
        return ;
    if(!chan->isChanOp(client)){
        // err;
        std::cerr << client->getNickName() << " is not with chanops\n";
        return;
    }
    if(!chan->isOnChan(*target)){
        // err;
        std::cerr << target->getNickName() << "is not in in the " << chan->getName() << '\n';
        return;
    }
    chan->removeClient(*client);
}

void sendInvite(Client *client, Client *target, Channel *chan){
    std::string msg = "INVITE " + target->getNickName() + " " + chan->getName() + "\n";
    sendMsg(target->getSocket(), msg);
}

void invite(Channel *chan, Client *client, Client *target){
    if(!chan || !chan->isChanOp(client)){
        std::cerr << "Not permitted\n";
        return;
    }
    if(chan->getMode().invite_only){
        if(chan->isOnChan(*target)){
            std::cerr << target->getNickName() << " is already in the channel\n";
            return;
        }
        sendInvite(client, target, chan);
        chan->addPendingClient(*target);
    }
}

void topic(Channel *chan, Client *client, std::string topic, int _do){
    if(chan->getMode().TopicRestricted && !chan->isChanOp(client)){
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