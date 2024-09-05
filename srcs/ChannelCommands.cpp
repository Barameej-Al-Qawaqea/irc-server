

#include "header.hpp"

// static const char *CHAN_NAME_ = "&#";


/* chanops commands */
// KICK    - Eject a client from the channel
// MODE    - Change the channel's mode
// INVITE  - Invite a client to an invite-only channel (mode +i)
// TOPIC   - Change the channel topic in a mode +t channel

bool join(Client *client, Channel *chan, std::string key){
    if(chan->isOnChan(*client)){
        sendMsg(client->getSocket(), ERR_USERONCHANNEL(chan->getName(), client->getNickName()));
        return false;
    }
    if(chan->getMode().invite_only && !chan->isPendingClient(*client)){
        sendMsg(client->getSocket(), ERR_INVITEONLYCHAN(chan->getName()));
        return false;
    }
    
    if(chan->getMode().UserLimit && (int)chan->getChanClients().size() >= chan->getlimit()){
        sendMsg(client->getSocket(), ERR_CHANNELISFULL(chan->getName()));
        return false;
    }
    if(chan->getMode().ChanReqPass && (chan->getPassword() != key)){
        sendMsg(client->getSocket(), ERR_BADCHANNELKEY(chan->getName()));
        return false;
    }
    // (chan->getMode().ChanReqPass && (chan->getPassword() != key))
    vector<Client> clients = chan->getChanClients();
    for(size_t i = 0; i < clients.size(); i++){
        sendMsg(clients[i].getSocket(), RPL_NAMREPLY(chan->getName(), client->getNickName()));
    }
    chan->AddToChan(*client);
    client->setcurrChan(chan);
    sendMsg(client->getSocket(), RPL_NAMREPLY(chan->getName(), client->getNickName()));
    return true;
}

void    mode(Channel *channel, Client *client, modeopt opt, std::vector<std::string> extra_params, \
                int _do, std::map<std::string, Client*>name_to_client) {
    
    std::vector<std::string>::iterator params = extra_params.begin();
    Client *clientTarget;

    if(!channel|| !channel->isChanOp(client)){
        if(!channel)
            sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(channel->getName()));
        else
            sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(channel->getName()));
        return;
    }
    params+=2;
    switch(opt){
        case INVITE_ONLY_OPT:
            if(*params != "+i" && *params != "-i"){
                sendMsg(client->getSocket(), ERR_UNKNOWNMODE(*params));
                return;
            }
            channel->set_remove_invite_only(client, _do);
            break;
        case TOPIC_RESTRICTION_OPT:
            channel->set_remove_topic_restriction(client, _do);
            break;
        case CHAN_KEY_OPT:
             if(params->size() < 2){
                sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("MODE")));
                return;
            }
            params++;
            channel->set_remove_channel_key(client, _do, *params);
            break;
        case CHANOP_OPT:
            if(params->size() < 2){
                sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("MODE")));
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
                sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(std::string("MODE")));
                return;
            }
            params++;
            channel->limitUserToChan(client, _do, std::atoi(params->c_str()));
            break;
        case UNKOWN:
            sendMsg(client->getSocket(), ERR_UNKNOWNMODE(*params));
            break;
    }
    
}

void kick(Client *client, Channel *chan, Client *target){
    if(!chan){
        sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(chan->getName()));
        return ;
    }
    if(!chan->isChanOp(client)){
        sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(chan->getName()));
        std::cerr << client->getNickName() << " is not with chanops\n";
        return;
    }
    if(!chan->isOnChan(*target)){
        sendMsg(client->getSocket(), ERR_USERNOTINCHANNEL(chan->getName(), target->getNickName()));
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
        if(!chan)
            sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(chan->getName()));
        else
            sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(chan->getName()));
        return;
    }
    if(chan->getMode().invite_only){
        if(chan->isOnChan(*target)){
            sendMsg(client->getSocket(), ERR_USERONCHANNEL(chan->getName(), target->getNickName()));
            return;
        }
        sendInvite(client, target, chan);
        chan->addPendingClient(*target);
    }
}

void topic(Channel *chan, Client *client, std::string topic, int _do){
    if(chan->getMode().TopicRestricted && !chan->isChanOp(client)){
        sendMsg(client->getSocket(), "ERR_CHANOPRIVSNEEDED\n");
        return;
    }
    if(_do){
        if(chan->getMode().TopicRestricted && !chan->isChanOp(client)){
            sendMsg(client->getSocket(), "ERR_CHANOPRIVSNEEDED\n");
            return;
        }
        chan->setTopic(topic);
    }
    else{
        string topic = chan->getTopic();
        if(topic.empty()){
            sendMsg(client->getSocket(), RPL_NOTOPIC(chan->getName()));
            return;
        }
        sendMsg(client->getSocket(), RPL_TOPIC(chan->getName(), chan->getTopic()));
    }

}