#include "header.hpp"

Channel *findChan(std::string name, std::deque<Channel *> channels,
                  bool &created) {
  for (size_t i = 0; i < channels.size(); i++) {
    if (channels[i]->getName() == name)
      return channels[i];
  }
  Channel *chan = new Channel(name);
  created = true;
  return chan;
}

void Command::executeJoin() {
  std::string name = cmd[1];
  bool created = false;
  if (cmd.size() < 2 || cmd[1].length() < 2 ||
      (cmd[1][0] != '#' && cmd[1][0] != '&')) {
    // ERR_NEEDMOREPARAMS
    sendMsg(client->getSocket(), std::string("invalid join params\n"));
    return;
  }
  // todo: skip # in the first name of the channel
  name = std::string(cmd[1].c_str() + 1);
  Channel *chan = findChan(name, this->serverData.channels, created);
  std::string key;

  if (cmd.size() == 3) {
    key = cmd[2];
  }
  if (join(this->client, chan, key) && created) {
    serverData.channels.push_back(chan);
  }
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
  if (cmd.size() == 1) {
    topic(this->client->getcurrChan(), this->client, "", 0);
  } else {
    topic(this->client->getcurrChan(), this->client, cmd[1], 1);
  }
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
