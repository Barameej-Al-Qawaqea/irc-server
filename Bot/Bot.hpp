#ifndef BOT_HPP
#define BOT_HPP
#include "MCTS.hpp"

class Bot {
    private :
        Game ticTacToe;
        Game gomuko;
        std::string ticTacToeDifficulty;
        std::string gomukoDifficulty;
        std::string msgToSend;
        
        bool    isInvalidBoardSize(std::string &num, std::string &gameType) const;
        bool    isInvalidCoordinates(std::string &X, std::string &Y) const;
        bool    isInvalidDifficulty(std::string &gameDifficulty) const;
        bool    isInvalidGameType(std::string &gameType) const;
        bool    isValidMove(std::string gameType, std::pair<int, int> move) const;
        bool    isTerminal(std::string gameType) const;
        bool    isGameFinished(Game &game, bool needPrint);
        void    startNewGame(std::string gameType, std::string gameDifficulty, int boardSize);
        void    playServerMove(std::string gameType);
        void    playClientMove(std::string gameType, std::pair <int, int> clientMove);

    public :
        Bot();
        std::string play(std::vector<std::string> &cmd);
        std::string botUsage() const;
        bool argumentsError(std::vector<std::string> &cmd) const;
        ~Bot();
};

#endif