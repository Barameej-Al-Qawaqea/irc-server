#include "header.hpp"
#include "Client.hpp"
class Command
{
    private :
        std::vector<std::string> cmd;
        Client *client;
        s_server_data &serverData;
        void    executePass() {}
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