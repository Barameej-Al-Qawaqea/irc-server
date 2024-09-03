#pragma once

#include "header.hpp"
#include "Client.hpp"

//PASS errors
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
//NICK erros
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_NOTREGISTERED 451
// privmsg
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_TOOMANYTARGETS 407
#define ERR_NOSUCHNICK 401

#include "header.hpp"

class Command
{
    private :
        std::vector<std::string> cmd;
        std::string originCmd;  // store original command, need it in privmsg
        Client *client;
        s_server_data &serverData;

        static std::string error(int errorNumber, std::string nickName) {
            std::string serverMsg = ":server ";
            switch (errorNumber) {
                case ERR_NEEDMOREPARAMS :
                    serverMsg += std::to_string(ERR_NEEDMOREPARAMS) + ' ' + nickName + " :Not enough parameters\n";
                    break;
                case ERR_ALREADYREGISTRED :
                    serverMsg += std::to_string(ERR_ALREADYREGISTRED) + ' ' + nickName + " :You may not reregister\n";
                    break;
                case ERR_PASSWDMISMATCH :
                    serverMsg += std::to_string(ERR_PASSWDMISMATCH) + ' ' + nickName + " :Password incorrect\n";
                    break;
                case ERR_NONICKNAMEGIVEN :
                    serverMsg += std::to_string(ERR_NONICKNAMEGIVEN) + ' ' + nickName + " :No nickname given\n";
                    break;
                case ERR_ERRONEUSNICKNAME :
                    serverMsg += std::to_string(ERR_NONICKNAMEGIVEN) + ' ' + nickName + " :Erroneous nickname\n";
                    break;
                case ERR_NICKNAMEINUSE :
                    serverMsg += std::to_string(ERR_NONICKNAMEGIVEN) + ' ' + nickName + " :Nickname is already in use\n";
                    break;
                case ERR_NOTREGISTERED :
                    serverMsg += std::to_string(ERR_NOTREGISTERED) + ' ' + nickName + " :You have not registered\n";
                case ERR_NORECIPIENT :
                    serverMsg += "411 " + nickName + " :No recipient given\n";
                    break;
                case ERR_NOSUCHNICK :
                    serverMsg += "401 " + nickName + " :No such nick/channel\n";
                    break;
            }
            return serverMsg;
        }   

        static bool validNickName(std::string &name) {
            std::string validCharacters = "-{}[]^`\\";
            bool validName = 1;
            for(size_t i = 0; i < name.size(); i++)
                validName &= (std::isalpha(name[i]) || validCharacters.find(name[i]) != std::string::npos);
            // std::cout << validName << ' ' << name << '\n';
            return validName;
        }

        void    executeBot() {

            std::string destination = client->getNickName().empty() ? "*" : client->getNickName();
            int sendReturn = 1;
            if (!client->isAlreadyRegistred())
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NOTREGISTERED, destination)) != -1;
            else if (client->argumentsError(cmd))
                sendReturn &= sendMsg(client->getSocket(), client->botUsage()) != -1;
            else sendReturn &= sendMsg(client->getSocket(), client->play(cmd)) != -1;
            if (!sendReturn)
                std::cerr << "Error occurs while sending message to the client\n";
        }

        void    executePass() {
            int sendReturn = 1;
            if (client->isAlreadyRegistred())
                sendReturn &= sendMsg(client->getSocket(), error(ERR_ALREADYREGISTRED, client->getNickName())) != -1;
            else if (cmd.size() == 1) {
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NEEDMOREPARAMS, "*")) != -1;
                client->unsetAuthenticated();  // only last pass command is used for verif
            }
            else if (cmd.size() > 2 || cmd[1] != serverData.password) {
                sendReturn &= sendMsg(client->getSocket(), error(ERR_PASSWDMISMATCH, "*")) != -1;
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
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NONICKNAMEGIVEN, destination)) != -1;
            else if (cmd.size() > 2 || !validNickName(cmd[1]))
                sendReturn &= sendMsg(client->getSocket(), error(ERR_ERRONEUSNICKNAME, destination)) != -1;
            else if (serverData.clientsNicknames.count(cmd[1])) // already inuse
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NICKNAMEINUSE, destination)) != -1;
            else if (!client->getAuthenticated()) // no PASS YEt
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NOTREGISTERED, destination)) != -1;
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
            (void)sendReturn;
            std::string destination = client->getNickName().empty() ? "*" : client->getNickName();
            std::cout << client->isAlreadyRegistred() << '\n';
            if (cmd.size() < 5 || (cmd.size() == 5 && cmd[4].size() == 1))
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NEEDMOREPARAMS, destination));
            else if (client->isAlreadyRegistred()) 
                sendReturn &= sendMsg(client->getSocket(), error(ERR_ALREADYREGISTRED, destination));
            else if (!client->getAuthenticated() || client->getNickName().empty())  // user didnt make a PASS,NICK yet
                sendReturn &= sendMsg(client->getSocket(),  error(ERR_NOTREGISTERED, destination));
            else {
                // all good
                client->setRegistred();
                sendMsg(client->getSocket(),  "Welcome " + client->getNickName() + " to ft_irc server\n");
                // todo
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

        bool    sendPrivMessage(std::vector<std::string> &toSend, std::string &message) {
            if (toSend.size() > 10 || !toSend.size() || message.empty()) return 1;
            std::cout << ":: " << toSend.size() << '\n';
            bool sendReturn = 1;
            for(size_t i = 0; i < toSend.size(); i++) {
                std::string receiver = toSend[i];
                if (isChannel(receiver)) {
                    // check if channel exist : ERR_NOSUCHNICK 
                    // check if clinet is in channel  : ERR_CANNOTSENDTOCHAN
                }
                else {
                    // check if client is connected to the server
                    if (clientExist(receiver)) {
                        int receiverFd = serverData.nameToClient[receiver]->getSocket();
                        sendReturn &= sendMsg(receiverFd, "Message from " + client->getNickName() + ": " + message + "\n") != -1;
                    }
                    else
                        sendReturn &= sendMsg(client->getSocket(), error(ERR_NOSUCHNICK, client->getNickName())) != -1;
                }
            }
            return sendReturn;
        }

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
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NORECIPIENT ,client->getNickName())) != -1;
            if (messageTosSend.empty())
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NOTEXTTOSEND, client->getNickName())) != -1;
            if (toSend.size() > 10)
                sendReturn &= sendMsg(client->getSocket(), error(ERR_TOOMANYTARGETS, client->getNickName())) != -1;
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
            else (this->*possibleFunctions[cmdIdx])();
        }
};



// what I need,  fdToClient
// need the serverData


//client class : username, nickname, isRegistred, isAuthenticated 
// PASS :
// 	syntax : PASS passwd
// 	errors: -ERR_NEEDMOREPARAMS
// 		    -ERR_ALREADYREGISTRED
// 		    -ERR_PASSWDMISMATCH
	

// NICK: 
// 	syntax : NICK nickname
// 		ERR_NONICKNAMEGIVEN
// 		ERR_ERRONEUSNICKNAME. (invalid nickname characters)
// 		ERR_NICKNAMEINUSE(already in use by someone else)
// 		ERR_NOTREGISTERED not authenticated yet(didnt make PASS yet)
// USER :
// 		ERR_NEEDMOREPARAMS
// 		ERR_ALREADYREGISTRED

// JOIN  :
// 	syntax : JOIN <channel>{,<channel>} [<key>{,<key>}]
// 	examples:
// 		JOIN #example,#example2 
// 		JOIN #example secretpassword
// 		JOIN  #example,#example2 secretpassword1,secretpassword2