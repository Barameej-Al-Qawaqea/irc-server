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

#include "header.hpp"

class Command
{
    private :
        std::vector<std::string> cmd;
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
            if (cmd.size() == 1)
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NEEDMOREPARAMS, "*")) != -1;
            else if (client->isAlreadyRegistred())
                sendReturn &= sendMsg(client->getSocket(), error(ERR_ALREADYREGISTRED, client->getNickName())) != -1;
            else if (cmd.size() > 2 || cmd[1] != serverData.password)
                sendReturn &= sendMsg(client->getSocket(), error(ERR_PASSWDMISMATCH, "*")) != -1;
            else {
                // command have the right passwd 
                client->setAuthenticated();
                // should send Some successfull reply that I dont currently know
                // sendReturn &= sendMsg(client->getSocket(), "some msg") != -1;
            }
            if (!sendReturn)
                std::cerr << "Error occurs while sending message to the client\n";
        }
        void    executeNick() {
            int sendReturn = 1;
            std::string destination = client->getNickName().empty() ? "*" : client->getNickName();   // destination should be '*' if user doenst have a nickname yet
            if (cmd.size() == 1)
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NONICKNAMEGIVEN, destination));
            else if (cmd.size() > 2 || !validNickName(cmd[1]))
                sendReturn &= sendMsg(client->getSocket(), error(ERR_ERRONEUSNICKNAME, destination));
            // else if (serverData.isNickNameInUse(cmd[1])) // isNickNameInUse: to code later
            //     sendReturn &= sendMsg(client->getSocket(), error(ERR_NICKNAMEINUSE, destination));
            else if (!client->getAuthenticated()) // no PASS YEt
                sendReturn &= sendMsg(client->getSocket(), error(ERR_NOTREGISTERED, destination));
            else {
                // all good

                // remove old nickname from serverData if it is not empty && add the newnickName to data && set the client new nickname
                // serverData.eraseNickName(client->getNickName());
                // serverData.addNickName(cmd[1]);
                client->setNickName(cmd[1]);
                // should send some msg (idont know right know)
                // sendReturn += sendMsg(client->getSocket(), "some msg");
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
                sendMsg(client->getSocket(),  "Welcome " + client->getNickName() + "to ft_irc server\n");
            }   
        }
        

        void    executeJoin() { }
        void    executeInvite() {}
        void    executeTopic() {}
        void    executeMode() {}
        void    executeKick() {}
    public :
        Command(std::string &command, Client *client, s_server_data &serverData) : client(client),  serverData(serverData)
        {
            std::stringstream ss(command);
            std::string word;
            while (ss >> word) this->cmd.push_back(word);
            // client->bot() = bot();
        }
        void    checkWhichCommand() {
            std::string possibleCommands[] = {"PASS", "NICK", "USER", "JOIN", "INVITE", "TOPIC", "MODE", "KICK", "BOT"};

            void(Command::*possibleFunctions[])() = {&Command::executePass, &Command::executeNick, &Command::executeUser,
                &Command::executeJoin, &Command::executeInvite, &Command::executeTopic,  &Command::executeMode, &Command::executeKick, &Command::executeBot};

            int cmdIdx = -1;
            for(int i = 0;i < cmd.size(); i++)
                std::cout << cmd[i] << '\n';
            for(int i = 0; i < 9; i++) {
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