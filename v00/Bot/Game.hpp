#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <sstream>
#include <iomanip>
#include <set>
#include <map>
#include <algorithm>
#include <cmath>

struct Game {
    int SIZE;
    std::string gameType;    // is it tictactoe or gomuko
    char currentPlayer;
    std::vector <std::string> board;
    bool    gameEnd;
    char winner;
    int movesLeft;
    std::vector<std::vector<int> > row;
    std::vector<std::vector<int> > col;
    std::vector<int> leftDiagonal;
    std::vector<int> rightDiagonal;
public:
    Game();
    Game(int SIZE, std::string &gameType);
    std::string                         printBoard() const;
    bool                                invalidMove(std::pair <int, int> &move) const;
    bool                                notStartedYet() const;
    std::pair<int, int>                 playEasy();
    std::pair<int, int>                 playMedium();
    char                                getWinner() const;
    void                                checkTicTacToeEnd(std::pair<int, int> &move);
    bool                                validIndexes(int x, int y) const;
    int                                 countConsecutiveCells(std::pair<int, int> &p, char player);
    void                                checkGomukoEnd(std::pair<int, int> &move);
    void                                playMove(std::pair <int, int> &move);
    bool                                isTerminal() const;
    char                                getCurrentPlayer() const;
    std::vector<std::pair<int, int> >   getPossibleMoves() const;
};

#endif