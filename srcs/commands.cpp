#include "header.hpp"

Command::Command(std::string &command, Client *client, s_server_data &serverData) : client(client),  serverData(serverData)
{
    this->originCmd = command;
    std::stringstream ss(command);
    std::string word;
    while (ss >> word) this->cmd.push_back(word);
}


void Command::executePart() {
    if (cmd.size() != 2) {
        sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), "PASS"));
    }
    std::deque<Channel *> &channels = this->serverData.channels;
  for(size_t i = 0; i < channels.size(); i++) {
    if (channels[i]->getName() == cmd[1].substr(1)) {
      part(this->client, channels[i],& channels);
      return;
    }
  }
}


void    Command::checkWhichCommand() {
    std::cout << originCmd << '\n';
    std::string possibleCommands[] = {"PASS", "NICK", "USER", "PRIVMSG", "JOIN", "PART" ,"INVITE", "TOPIC", "MODE", "KICK", "BOT"};
        void(Command::*possibleFunctions[])() = {&Command::executePass, &Command::executeNick, &Command::executeUser, &Command::executePrivmsg,
        &Command::executeJoin, &Command::executePart , &Command::executeInvite, &Command::executeTopic,  &Command::executeMode, &Command::executeKick, &Command::executeBot};

    int cmdIdx = -1;
    for(int i = 0; i < 11; i++) {
        if (!cmd.empty() && cmd[0] == possibleCommands[i])
            cmdIdx = i;
    }
    if (cmdIdx == -1) {
        // command not found -> should send some error to the client
    }
    else {
        if (cmdIdx >= 3) {
          if (!client->isAlreadyRegistred()) {
            sendMsg(client->getSocket(), ERR_NOTREGISTERED(((client->getNickName().empty()) ? "*" : client->getNickName())));
            return ;
          }
        }
        (this->*possibleFunctions[cmdIdx])();
    }
}

Channel *findChan(std::string name, std::deque<Channel *> &channels,
                  bool &created) {
  for (size_t i = 0; i < channels.size(); i++) {
    if (channels[i]->getName() == name)
      return channels[i];
  }
  if(created){
    created = false;
    return NULL;
  }
  Channel *chan = new Channel(name);
  created = true;
  return chan;
}

void getChans(std::string cmd,std::vector<std::pair<std::string, std::string> > &chanName_chanKey) {
  std::stringstream ss(cmd);
  std::string token;
 
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
  getChans(cmd[1], chanName_chanKey);
  if (cmd.size() == 3) {
    getKeys(cmd[2]  , chanName_chanKey);
  }
  for (size_t i = 0; i < chanName_chanKey.size(); i++) {
    if((chanName_chanKey[i].first[0] != '#' && chanName_chanKey[i].first[0] != '&' )|| chanName_chanKey[i].first.length() < 2){
      sendMsg(client->getSocket(), ERR_NOSUCHCHANNEL(client->getNickName(), chanName_chanKey[i].first));
      continue;
    }
    Channel *chan = findChan(chanName_chanKey[i].first.substr(1), this->serverData.channels, created);
    if (join(this->client, chan, chanName_chanKey[i].second) && created) {
      serverData.channels.push_back(chan);
    }
    created = false;
  }
}

void Command::executeInvite() {
  bool created = false;
  if (cmd.size() != 3) {
    sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), "INVITE"));
    return;
  }
  if (this->serverData.nameToClient.find(cmd[1]) ==
      this->serverData.nameToClient.end())
    sendMsg(client->getSocket(), std::string("client dosent exist\n"));
  if (cmd[2].length() > 1) {
    if (cmd[2][0] != '#')
      return;
    std::string chanName = cmd[2].substr(1);
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
       get_which_opt(cmd, ((cmd.size() < 2) * -1), plus), cmd, plus,
       this->serverData.nameToClient, cmd.size() );
}

void Command::executeKick() {
  if (cmd.size() == 3 || cmd.size() == 4) {
    kick(this->client, this->client->getcurrChan(),
         this->serverData.nameToClient[cmd[2]], cmd.size() == 4 ? cmd[3] : "", this->serverData.channels \
         , cmd[1], cmd[2]);
  }
  else
    sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(client->getNickName(), "KICK"));
}

void    Command::executeBot() {
	std::string message, line;
    
	if (client->argumentsError(cmd))
		message = client->botUsage();
	else
		message = client->play(cmd);
	std::stringstream ss(message);
	while (std::getline(ss, line, '\n'))
		sendMsg(client->getSocket(), "001 "+ client->getNickName() + " :" + line + "\r\n");
	sendMsg(client->getSocket(), "001 "+ client->getNickName() + " :\r\n" );
}
