

#include "header.hpp"

// static const char *CHAN_NAME_ = "&#";


/* chanops commands */
// KICK    - Eject a client from the channel
// MODE    - Change the channel's mode
// INVITE  - Invite a client to an invite-only channel (mode +i)
// TOPIC   - Change the channel topic in a mode +t channel

std::string getClientNames(std::vector<Client> clients){
    std::string names;
    for(size_t i = 0; i < clients.size(); i++){
        names += clients[i].getNickName();
        if (i != clients.size() - 1)
            names += " ";
    }
    return names;
}

bool join(Client *client, Channel *chan, std::string key){
    if(chan->isOnChan(*client)){
        sendMsg(client->getSocket(), ERR_USERONCHANNEL(chan->getName(), client->getNickName()));
        return false;
    }
    if(chan->getMode().invite_only && !chan->isPendingClient(*client)){
        sendMsg(client->getSocket(), ERR_INVITEONLYCHAN(client->getNickName(), chan->getName()));
        return false;
    }
    
    if(chan->getMode().UserLimit && (int)chan->getChanClients().size() >= chan->getlimit()){
        sendMsg(client->getSocket(), ERR_CHANNELISFULL(client->getNickName(), chan->getName()));
        return false;
    }
    if(chan->getMode().ChanReqPass && (chan->getPassword() != key)){
        sendMsg(client->getSocket(), ERR_BADCHANNELKEY(client->getNickName(), chan->getName()));
        return false;
    }
    // (chan->getMode().ChanReqPass && (chan->getPassword() != key))
    vector<Client> clients = chan->getChanClients();
    for(size_t i = 0; i < clients.size(); i++){
        // sendMsg(clients[i].getSocket(), RPL_NAMREPLY(chan->getName(), client->getNickName(), getClientNames(chan->getChanClients())));
        sendMsg(clients[i].getSocket(), RPL_JOIN(client->getNickName(), client->getuserName(), client->getHostName(), chan->getName()));
    }
    chan->AddToChan(*client);
    client->setcurrChan(chan);
        // sendMsg(client->getSocket(), RPL_NAMREPLY(chan->getName(), client->getNickName()));
    sendMsg(client->getSocket(), RPL_JOIN(client->getNickName(), client->getuserName(), client->getHostName(), chan->getName()));
    if(!chan->getTopic().empty())
        sendMsg(client->getSocket(), RPL_TOPIC(chan->getName(), chan->getTopic()));
    // sendMsg(client->getSocket(), RPL_TOPIC(chan->getName(), chan->getTopic()));
    sendMsg(client->getSocket(), RPL_NAMREPLY(chan->getName(), client->getNickName(),getClientNames(chan->getChanClients())));
    sendMsg(client->getSocket(), RPL_ENDOFNAMEST(chan->getName(), client->getNickName()));
    return true;
}

void    mode(Channel *channel, Client *client, modeopt opt, std::vector<std::string> extra_params, \
                int _do, std::map<std::string, Client*>name_to_client) {
    
    std::vector<std::string>::iterator params = extra_params.begin();
    Client *clientTarget;

    if(!channel|| !channel->isChanOp(client)){
        if(!channel)
            sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickName(),channel->getName()));
        else
            sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(client->getNickName(), channel->getName()));
        return;
    }
    params+=2;
    switch(opt){
        case INVITE_ONLY_OPT:
            if(*params != "+i" && *params != "-i"){
                sendMsg(client->getSocket(), ERR_UNKNOWNMODE(client->getNickName(), *params));
                return;
            }
            channel->set_remove_invite_only(client, _do);
            break;
        case TOPIC_RESTRICTION_OPT:
            channel->set_remove_topic_restriction(client, _do);
            break;
        case CHAN_KEY_OPT:
             if(params->size() < 2){
                sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), std::string("MODE")));
                return;
            }
            params++;
            channel->set_remove_channel_key(client, _do, *params);
            break;
        case CHANOP_OPT:
            if(params->size() < 2){
                sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), std::string("MODE")));
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
                sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName() ,std::string("MODE")));
                return;
            }
            params++;
            channel->limitUserToChan(client, _do, std::atoi(params->c_str()));
            break;
        case UNKOWN:
            sendMsg(client->getSocket(), ERR_UNKNOWNMODE(client->getNickName(), *params));
            break;
    }
    
}

void kick(Client *client, Channel *chan, Client *target, std::string targetName, std::string reason){
    if(!chan){
        sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickName(), chan->getName()));
        return ;
    }
    if(!target){
        sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), ));
        return;
    }
    if(!chan->isChanOp(client)){
        sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(client->getNickName(), chan->getName()));
        return;
    }
    if(!chan->isOnChan(*target)){
        sendMsg(client->getSocket(), ERR_USERNOTINCHANNEL(chan->getName(), target->getNickName()));
        return;
    }
    chan->removeClient(*target);
    sendMsg(client->getSocket(),\
     ":" + client->getNickName()+ "!~" + client->getuserName() + "@" + client->getHostName() + \
      " KICK #" + chan->getName() + " " + target->getNickName() + " :" + reason + "\n");
}

void sendInvite(Client *client, Client *target, Channel *chan){
    std::string msg = RPL_INVITING(chan->getName(), client->getNickName());
    (void)client;
    sendMsg(target->getSocket(), msg);
}

void invite(Channel *chan, Client *client, Client *target){
    if(!chan || !chan->isChanOp(client)){
        if(!chan)
            sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickName(), chan->getName()));
        else
            sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(client->getNickName(), chan->getName()));
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

void topic(Channel *chan, Client *client, std::vector<string>params){
    if(params.size() < 2){
        sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), std::string("TOPIC")));
        return;
    }
    if(!chan){
        sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickName(), chan->getName()));
        return;
    }
    if(chan->getMode().TopicRestricted && !chan->isChanOp(client) && params.size() == 3){
        sendMsg(client->getSocket(), ERR_CHANOPRIVSNEEDED(client->getNickName(), chan->getName()));
        return;
    }
    if(params.size() == 3){
        if(params[2][0]!= ':'){
            sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), std::string("TOPIC")));
            return;
        }
        chan->setTopic(params[2].substr(1));
    }
    else{
        string topic = chan->getTopic();
        if(topic.empty()){
            sendMsg(client->getSocket(), RPL_NOTOPIC(chan->getName(), client->getNickName()));
            return;
        }
        sendMsg(client->getSocket(), RPL_TOPIC(chan->getName(), chan->getTopic()));
    }
}
