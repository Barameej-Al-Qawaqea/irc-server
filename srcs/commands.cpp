#include "header.hpp"


Channel *findChan(std::string name, std::deque<Channel *> channels,
                  bool &created) {
  for (size_t i = 0; i < channels.size(); i++) {
    if (channels[i]->getName() == name)
      return channels[i];
  }
  if(created){
    created = false;
    std::cout << "channel already exist\n";
    return NULL;
  }
  Channel *chan = new Channel(name);
  created = true;
  return chan;
}

void getChans(std::string cmd,std::vector<std::pair<std::string, std::string> > &chanName_chanKey) {
  std::stringstream ss(cmd);
  std::string token;
 
  int i = 0;
  while(getline(ss, token, ',')){
    chanName_chanKey.push_back(std::make_pair(token, ""));
  } 
}

void getKeys(std::string cmd, std::vector<std::pair<std::string, std::string> > &chanName_chanKey) {
  std::stringstream ss(cmd);
  std::string token;
  int i = 0;
  while(getline(ss, token, ',')){
    chanName_chanKey[i].second = token;
    i++;
  } 
}


void Command::executeJoin() {
  std::string name = cmd[1];
  bool created = false;
  std::vector<std::pair<std::string, std::string> >chanName_chanKey ;

  if (cmd.size() != 2 && cmd.size() != 3) {
    sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), "JOIN"));
    return;
  }
  // todo: skip # in the first name of the channel
  getChans(cmd[1], chanName_chanKey);
  if (cmd.size() == 3) {
    getKeys(cmd[2]  , chanName_chanKey);
  }
  for (size_t i = 0; i < chanName_chanKey.size(); i++) {
    if(chanName_chanKey[i].first[0] != '#' && chanName_chanKey[i].first[0] != '&'){
      sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickName(), chanName_chanKey[i].first));
      continue;
    }
    Channel *chan = findChan(chanName_chanKey[i].first.substr(1), this->serverData.channels, created);
    if (join(this->client, chan, chanName_chanKey[i].second) && created) {
      serverData.channels.push_back(chan);
    }
    created = false;
  }
  // if (join(this->client, chan, key) && created) {
  //   serverData.channels.push_back(chan);
  // }
}

void Command::executeInvite() {
  bool created = false;
  if (cmd.size() != 3) {
    sendMsg(client->getSocket(), std::string("invalid invite params\n"));
    return;
  }
  if (this->serverData.nameToClient.find(cmd[1]) ==
      this->serverData.nameToClient.end())
    sendMsg(client->getSocket(), std::string("client dosent exist\n"));
  if (cmd[2].length() > 1) {
    if (cmd[2][0] != '#')
      return;
    std::string chanName = std::string(cmd[2].c_str() + 1);
    Channel *chan = findChan(chanName, this->serverData.channels, created);
    invite(chan, this->client, this->serverData.nameToClient[cmd[1]]);
  }
}

void Command::executeTopic() {
    topic(this->client->getcurrChan(), this->client, cmd);
}

void Command::executeMode() {
  int plus;
  mode(this->client->getcurrChan(), this->client,
       get_which_opt(cmd, ((cmd.size() < 3) * -1), plus), cmd, plus,
       this->serverData.nameToClient);
}

void Command::executeKick() {
  if (cmd.size() == 3)
    kick(this->client, this->client->getcurrChan(),
         this->serverData.nameToClient[cmd[1]]);
}
