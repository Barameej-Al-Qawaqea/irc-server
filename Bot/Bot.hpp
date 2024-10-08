#ifndef BOT_HPP
#define BOT_HPP
#include "MCTS.hpp"

class Bot {
    private :
        Game        ticTacToe;
        Game        gomuko;
        std::string ticTacToeDifficulty;
        std::string gomukoDifficulty;
        std::string msgToSend;
        
        static bool     isInvalidBoardSize(std::string &num, std::string &gameType);
        static bool     isInvalidCoordinates(std::string &X, std::string &Y);
        static bool     isInvalidDifficulty(std::string &gameDifficulty);
        static bool     isInvalidGameType(std::string &gameType);
        static bool     isInvalidLogin(std::string &login);
        bool            isValidMove(std::string gameType, std::pair<int, int> move) const;
        bool            isTerminal(std::string gameType) const;
        bool            isGameFinished(Game &game, bool needPrint);
        void            startNewGame(std::string gameType, std::string gameDifficulty, int boardSize);
        void            playServerMove(std::string gameType);
        void            playClientMove(std::string gameType, std::pair <int, int> clientMove);

    public :
        Bot();
        std::string         play(std::vector<std::string> &cmd);
        static  std::string botUsage();
        static  bool        argumentsError(std::vector<std::string> &cmd);
        ~Bot();
};

#endif