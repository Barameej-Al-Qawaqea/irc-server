#pragma once

#include "header.hpp"
#include "Client.hpp"
#include "Replies.hpp"

Channel *findChan(std::string name, std::deque<Channel *> channels,
                  bool &created);

class Command
{
    private :
        std::vector<std::string> cmd;
        std::string originCmd;  // store original command, need it in privmsg
        Client *client;
        s_server_data &serverData;

        static bool validNickName(std::string &name) {
            std::string validCharacters = "-{}[]^`\\";
            bool validName = 1;
            for(size_t i = 0; i < name.size(); i++)
                validName &= (std::isalpha(name[i]) || validCharacters.find(name[i]) != std::string::npos);
            return validName;
        }

        void    executeBot() {

            std::string destination = client->getNickName().empty() ? "*" : client->getNickName();
            int sendReturn = 1;
            if (!client->isAlreadyRegistred())
                sendReturn &= sendMsg(client->getSocket(), ERR_NOTREGISTERED(destination)) != -1;
            else if (client->argumentsError(cmd))
                sendReturn &= sendMsg(client->getSocket(), client->botUsage()) != -1;
            else sendReturn &= sendMsg(client->getSocket(), client->play(cmd)) != -1;
            if (!sendReturn)
                std::cerr << "Error occurs while sending message to the client\n";
        }

        void    executePass() {
            int sendReturn = 1;
            if (client->isAlreadyRegistred())
                sendReturn &= sendMsg(client->getSocket(), ERR_ALREADYREGISTRED(client->getNickName())) != -1;
            else if (cmd.size() == 1) {
                sendReturn &= sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS((string)"*", (string)"PASS")) != -1;
                client->unsetAuthenticated();  // only last pass command is used for verif
            }
            else if (cmd.size() > 2 || cmd[1] != serverData.password) {
                sendReturn &= sendMsg(client->getSocket(), ERR_PASSWDMISMATCH((string)"*")) != -1;
                // this is according to rfc1459, only last pass command is used for verification
                client->unsetAuthenticated(); 
            }
            else
                client->setAuthenticated();
            if (!sendReturn)
                std::cerr << "Error occurs while sending message to the client\n";
        }
        void    executeNick() {
            int sendReturn = 1;
            std::string destination = client->getNickName().empty() ? "*" : client->getNickName();   // destination should be '*' if user doenst have a nickname yet
            if (cmd.size() == 1)
                sendReturn &= sendMsg(client->getSocket(), ERR_NONICKNAMEGIVEN(destination)) != -1;
            else if (cmd.size() > 2 || !validNickName(cmd[1]))
                sendReturn &= sendMsg(client->getSocket(), ERR_ERRONEUSNICKNAME(destination)) != -1;
            else if (serverData.clientsNicknames.count(cmd[1])) // already inuse
                sendReturn &= sendMsg(client->getSocket(), ERR_NICKNAMEINUSE(destination)) != -1;
            else if (!client->getAuthenticated()) // no PASS YEt
                sendReturn &= sendMsg(client->getSocket(), ERR_NOTREGISTERED(destination)) != -1;
            else {
                if (serverData.clientsNicknames.count(client->getNickName()))
                    serverData.clientsNicknames.erase(client->getNickName());
                serverData.clientsNicknames.insert(cmd[1]);
                // should change his name in nameToClient too if he is fully registered
                if (client->isAlreadyRegistred()) {
                    serverData.nameToClient.erase(client->getNickName());
                    serverData.nameToClient[cmd[1]] = client;
                }
                client->setNickName(cmd[1]);
                sendReturn += sendMsg(client->getSocket(), cmd[1] + " successfully set a new nickname\n") != -1;
            }
            if (!sendReturn)
                std::cerr << "Error occurs while sending message to the client\n";
        }

        void    executeUser() {
            int sendReturn = 1;
            std::string destination = client->getNickName().empty() ? "*" : client->getNickName();
            if (cmd.size() != 5)
                sendReturn &= sendMsg(client->getSocket(), ERR_NEEDMOREPARAMS(destination, "USER"));
            else if (client->isAlreadyRegistred()) 
                sendReturn &= sendMsg(client->getSocket(), ERR_ALREADYREGISTRED(destination));
            else if (!client->getAuthenticated() || client->getNickName().empty())  // user didnt make a PASS,NICK yet
                sendReturn &= sendMsg(client->getSocket(),  ERR_NOTREGISTERED(destination));
            else {
                // all good
                client->setRegistred();
                sendMsg(client->getSocket(),  "Welcome " + client->getNickName() + " to ft_irc server\n");
                serverData.nameToClient[client->getNickName()] = client;
            }   
        }

        std::vector<std::string> getUsersAndChannels(void) {
            std::vector<std::string> toSend;
            size_t i = 0;
            while (isspace(originCmd[i])) i++;
            while (i < originCmd.size() && !isspace(originCmd[i])) i++;
            while (i < originCmd.size() && isspace(originCmd[i])) i++;
            originCmd.erase(originCmd.begin(), originCmd.begin() + i);
            while (originCmd.size() && isspace(originCmd.back())) originCmd.pop_back();

            if (originCmd.empty()) return toSend;
            std::stringstream ss(originCmd);
            std::string newUser;
            while (getline(ss, newUser, ','))
                toSend.push_back(newUser);
            return toSend;
        }
    
        bool isChannel(const std::string &msg){
            return (msg[0] == '#' || msg[0] == '&');
        }

        bool clientExist(std::string &name) {
            return serverData.nameToClient.count(name);
        }

        bool    duplicateExist(std::vector<std::string> &toSend) {
            std::set <std::string> targets;
            for (size_t i = 0; i < toSend.size(); i++) {
                if (targets.count(toSend[i])) return 1;
                targets.insert(toSend[i]);
            }
            return 0;
        }
        
        bool    sendPrivMessage(std::vector<std::string> &toSend, std::string &message) {
            if (toSend.size() > 10 || !toSend.size() || message.empty()) return 1;
            bool sendReturn = 1;
            if (duplicateExist(toSend))
                return sendMsg(client->getSocket(), ERR_TOOMANYTARGETS(client->getNickName(), originCmd, 1)) != -1;
            for(size_t i = 0; i < toSend.size(); i++) {
                std::string receiver = toSend[i];
                if (isChannel(receiver)) {
                    // check if channel exist : ERR_NOSUCHNICK 
                    bool created = 1;
                    Channel *chan = findChan(receiver.substr(1), serverData.channels, created);
                    if (!created)
                        sendReturn &= sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), receiver)) != -1;
                    else {
                        // check if clinet is in channel  : ERR_CANNOTSENDTOCHAN
                        if(!chan->isOnChan(*client))
                            //! ERR_CANNOTSENDTOCHAN
                            sendReturn &= sendMsg(client->getSocket(), ERR_CANNOTSENDTOCHAN(client->getNickName(), receiver)) != -1;
                        else {
                            std::vector<Client> ChanClien = chan->getChanClients();
                            for(size_t i = 0; i < ChanClien.size(); i++) {
                                if (ChanClien[i].getSocket() == client->getSocket()) continue;    
                                sendReturn &= sendMsg(ChanClien[i].getSocket(), "Message from " + client->getNickName() + ": " + message + "\n") != -1;
                            }
                        }
                    }
                }
                else {
                    // check if client is connected to the server
                    if (clientExist(receiver)) {
                        int receiverFd = serverData.nameToClient[receiver]->getSocket();
                        sendReturn &= sendMsg(receiverFd, "Message from " + client->getNickName() + ": " + message + "\n") != -1;
                    }
                    else
                        sendReturn &= sendMsg(client->getSocket(), ERR_NOSUCHNICK(client->getNickName(), receiver)) != -1;
                }
            }
            return sendReturn;
        }
 
        // std::string getTargets(std::vector<std::string> &toSend) {
        //     std::string targets;
        //     for (size_t i = 0; i < toSend.size(); i++) {
        //         targets += toSend[i];
        //         if ()
        //     }
        //     return targets;
        // }

        void    executePrivmsg() {
            std::string messageTosSend = "";
            size_t msgIdx = originCmd.find(':');
            if (msgIdx != std::string::npos) {
                messageTosSend = originCmd.substr(msgIdx + 1);
                originCmd.erase(originCmd.begin() + msgIdx, originCmd.end());
            }
            std::vector<std::string> toSend = getUsersAndChannels();

            // std::cout << messageTosSend << '\n';
            // for(size_t i = 0; i < toSend.size(); i++)
            //     std::cout << toSend[i] << ' ';
            // std::cout << '\n';
            int sendReturn = 1;
            if (toSend.empty())
                sendReturn &= sendMsg(client->getSocket(), ERR_NORECIPIENT(client->getNickName())) != -1;
            if (messageTosSend.empty())
                sendReturn &= sendMsg(client->getSocket(), ERR_NOTEXTTOSEND(client->getNickName())) != -1;
            if (toSend.size() > 10)
                sendReturn &= sendMsg(client->getSocket(), ERR_TOOMANYTARGETS(client->getNickName(), originCmd, 0)) != -1;
            sendReturn &= sendPrivMessage(toSend, messageTosSend);
            if (!sendReturn)
                std::cerr << "Error occurs while sending message to the client\n";
        }

        void    executeJoin();
        void    executeInvite();
        void    executeTopic();
        void    executeMode();
        void    executeKick();
    public :
        Command(std::string &command, Client *client, s_server_data &serverData) : client(client),  serverData(serverData)
        {
            this->originCmd = command;
            std::stringstream ss(command);
            std::string word;
            while (ss >> word) this->cmd.push_back(word);
        }

        void    checkWhichCommand() {
            std::cout << originCmd << '\n';
            std::string possibleCommands[] = {"PASS", "NICK", "USER", "PRIVMSG", "JOIN", "INVITE", "TOPIC", "MODE", "KICK", "BOT"};
                void(Command::*possibleFunctions[])() = {&Command::executePass, &Command::executeNick, &Command::executeUser, &Command::executePrivmsg,
                &Command::executeJoin, &Command::executeInvite, &Command::executeTopic,  &Command::executeMode, &Command::executeKick, &Command::executeBot};

            int cmdIdx = -1;
            for(int i = 0; i < 10; i++) {
                if (!cmd.empty() && cmd[0] == possibleCommands[i])
                    cmdIdx = i;
            }
            if (cmdIdx == -1) {
                // command not found -> should send some error to the client
            }
            else {
                if (cmdIdx >= 3) {
                    if (!client->isAlreadyRegistred()) {
                        if (sendMsg(client->getSocket(), ERR_NOTREGISTERED(((client->getNickName().empty()) ? "*" : client->getNickName()))) == -1)
                            std::cerr << "Error occurs while sending message to the client\n";
                        return ;
                    }
                }
                (this->*possibleFunctions[cmdIdx])();
            }
        }
};
