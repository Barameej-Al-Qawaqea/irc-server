#include "header.hpp"
#include "Client.hpp"

#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
class Command
{
    private :
        std::vector<std::string> cmd;
        Client *client;
        s_server_data &serverData;

        std::string error(int errorNumber, std::string nickName) {
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
                // case 
            }
            return serverMsg;
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
        void    executeNick() {}
        void    executeUser() {}
        void    executeJoin() {}
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
        }
        void    checkWhichCommand() {
            std::string possibleCommands[8] = {"PASS", "NICK", "USER", "JOIN", "INVITE", "TOPIC", "MODE", "KICK"};

            void(Command::*possibleFunctions[8])() = {&Command::executePass, &Command::executeNick, &Command::executeUser,
                &Command::executeJoin, &Command::executeInvite, &Command::executeTopic,  &Command::executeMode, &Command::executeKick};

            int cmdIdx = -1;
            for(int i = 0; i < 8; i++) {
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
// 		an error not mentioned in rfc : what if user is not authenticated yet
// USER :
// 		ERR_NEEDMOREPARAMS
// 		ERR_ALREADYREGISTRED

// JOIN  :
// 	syntax : JOIN <channel>{,<channel>} [<key>{,<key>}]
// 	examples:
// 		JOIN #example,#example2 
// 		JOIN #example secretpassword
// 		JOIN  #example,#example2 secretpassword1,secretpassword2