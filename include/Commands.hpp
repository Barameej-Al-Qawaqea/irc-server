#ifndef COMMANDS_H
# define COMMANDS_H

#include "header.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#define MAX_TARGETS 10
class Command
{
    private :
        std::vector<std::string>    cmd;
        std::string                 originCmd;  // store original command, need it in privmsg
        Client                      *client;
        s_server_data               &serverData;

        void                        executePass();
        void                        executeNick();
        void                        executeUser();
        void                        executePrivmsg();
        void                        executeJoin();
        void                        executeInvite();
        void                        executeTopic();
        void                        executeMode();
        void                        executeKick();
        void                        executeBot();

        bool                        validNickName(std::string &name) const;
        bool                        isChannel(const std::string &msg) const;
        bool                        clientExist(std::string &name) const;
        bool                        duplicateExist(std::vector<std::string> &toSend) const;
        Channel*                    getChannel(std::string name) const;
        std::vector<std::string>    getUsersAndChannels();
        void                        sendMsgToChannelClients(std::string &reciver, std::string &message) const;
        void                        sendPrivMessage(std::vector<std::string> &toSend, std::string &message) const;
        void                        notifyNickChangeToChannels(const std::string &oldName, const std::string &name) const;
        void                        notifyActiveChats(const std::string &oldname, const std::string &newName) const;

        // i want to create a function that take a string which have command and return a pointer to a class Command function to the same command
        // i just want to use it for tests
        
        
    public :
        Command(std::string &command, Client *client, s_server_data &serverData);
        void    checkWhichCommand();
};

#endif
