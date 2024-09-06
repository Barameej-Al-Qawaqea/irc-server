#include "Game.hpp"

Game::Game() : SIZE(0), gameEnd(0) {}
Game::Game(int SIZE, std::string &gameType) : SIZE(SIZE), gameType(gameType), currentPlayer('X'), board(SIZE, std::string(SIZE, '.')) {
    movesLeft = SIZE * SIZE;
    winner = 'N';
    gameEnd = 0;
    if (gameType == "TICTACTOE") {
        row = std::vector <std::vector<int> > (SIZE, std::vector<int> (2));
        col = std::vector <std::vector<int> > (SIZE, std::vector<int> (2));
        leftDiagonal = std::vector<int>(2);
        rightDiagonal = std::vector<int>(2);
    }
}

std::string Game::printBoard() const {
    std::ostringstream oss;

    oss << "   ";
    for(int i = 0; i < SIZE; i++)
        oss << std::setw(3) << i << ' ';
    oss << '\n';
    oss << "   ";
    for (int j = 0; j < SIZE; ++j)
        oss << std::setw(4) << "---";
    oss << '\n';

    for (int i = 0; i < SIZE; ++i) {
        oss << std::setw(2) << i << " ";

        for (int j = 0; j < SIZE; ++j)
            oss << "| " << board[i][j] << " ";
        oss << "|\n" ;
        oss << "   ";
        for (int j = 0; j < SIZE; ++j)
            oss << std::setw(4) << "---";
        oss << '\n';
    }
    return oss.str();
}

bool    Game::invalidMove(std::pair <int, int> &move) const {
    return (move.first >= SIZE || move.second >= SIZE || board[move.first][move.second] != '.');
}
bool    Game::notStartedYet() const {
    return (SIZE == 0);
}

std::pair<int, int> Game::playEasy() {
    std::vector<std::pair<int, int> > possibleMoves = getPossibleMoves();
    std::pair <int, int> move = possibleMoves[rand() % possibleMoves.size()];
    return move;
}

std::pair<int, int> Game::playMedium() {
    std::set<std::vector<int>, std::greater<std::vector<int> > > maxConsecutives;  // vector = {count , player, x, y} 
    for(int x = 0; x < SIZE; x++) {
        for(int y = 0; y < SIZE; y++) {
            if (board[x][y] != '.') continue;
            else {
                std::pair<int, int> cell = std::make_pair(x, y);
                char player = 'O';
                board[x][y] = player;
                // take the cell the have the maxconsecutive count , if 2 cells have same maxconsecu take the one where you are the currentPlayer
                int consecutives = countConsecutiveCells(cell, player);
                int arr[] = {consecutives, player, x, y};
                maxConsecutives.insert(std::vector<int>(arr, arr + 4));
                player = 'X';
                board[x][y] = player;
                consecutives = countConsecutiveCells(cell, player);
                arr[0] = consecutives, arr[1] = player;
                maxConsecutives.insert(std::vector<int>(arr, arr + 4));
                board[x][y] = '.';
            }
        }
    }
    std::vector<std::pair<int, int> > botMoves;
    std::vector<int> first = *maxConsecutives.begin();
    for(std::set<std::vector<int> >::iterator it = maxConsecutives.begin(); it != maxConsecutives.end(); it++) {
        std::vector<int> cur = *it;
        if (cur[0] == first[0] && cur[1] == first[1])
            botMoves.push_back(std::make_pair(cur[2], cur[3]));
        else break;
    }
    int idx = rand() % botMoves.size();
    return botMoves[idx];
}

char Game::getWinner() const {
    return winner;
}

void    Game::checkTicTacToeEnd(std::pair<int, int> &move) {
    
    int player = (currentPlayer == 'X') ? 1 : 0;
    row[move.first][player] += 1;
    col[move.second][player] += 1;
    movesLeft--;
    if (move.first == move.second)
        leftDiagonal[player]++;
    if (move.first == SIZE - 1 - move.second)
        rightDiagonal[player]++;
    if (row[move.first][player] == SIZE || col[move.second][player] == SIZE
        || leftDiagonal[player] == SIZE || rightDiagonal[player] == SIZE) {
        gameEnd = 1;
        winner = currentPlayer;
    }
    else if (!movesLeft) gameEnd = 1;
}

bool    Game::validIndexes(int x, int y) const {
    return (x >= 0 && y >= 0 && x < SIZE && y < SIZE);
}

int Game::countConsecutiveCells(std::pair<int, int> &p, char player) {
    static int side1x[4] = {0, -1, -1, 1};   //x
    static int side1y[4] = {1, 0, 1, 1};  //y

    static int side2x[4] = {0, 1, 1, -1};
    static int side2y[4] = {-1, 0, -1, -1};
    int upperBound = gameType != "GOMUKO" ? SIZE : 5;
    int maxCount = 0;
    for(int k = 0; k < 4; k++) {
        int count = 0;
        int x = p.first, y = p.second;
        while (count < upperBound && validIndexes(x, y) && board[x][y] == player) {
            count++;
            x += side1x[k];
            y += side1y[k];
        }
        x = p.first + side2x[k], y = p.second + side2y[k];
        while (count < upperBound && validIndexes(x, y) && board[x][y] == player) {
            count++;
            x += side2x[k];
            y += side2y[k];
        }
        if (count >= upperBound) return count;
        else maxCount = std::max(maxCount, count);
    }
    return maxCount;
}

void    Game::checkGomukoEnd(std::pair<int, int> &move) {
    movesLeft--;
    static const int winCondition = 5;

    if (countConsecutiveCells(move, currentPlayer) >= winCondition) {
        gameEnd = 1;
        winner = currentPlayer;
    }
    else if (!movesLeft) gameEnd = 1;
}

void    Game::playMove(std::pair <int, int> &move) {
    board[move.first][move.second] = currentPlayer;
    if (gameType == "TICTACTOE")
        checkTicTacToeEnd(move);
    else checkGomukoEnd(move);
    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
}

bool    Game::isTerminal() const {
    return gameEnd;
}

char    Game::getCurrentPlayer() const {
    return currentPlayer;
}

std::vector<std::pair<int, int> >   Game::getPossibleMoves() const {
    std::vector<std::pair<int, int> > possibleMoves;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == '.') {
                possibleMoves.push_back(std::make_pair(i, j));
            }
        }
    }
    return possibleMoves;
}
