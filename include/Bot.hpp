#pragma once
#include "header.hpp"

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
    Game() : SIZE(0), gameEnd(0) {}
    Game(int SIZE, std::string &gameType) : SIZE(SIZE), gameType(gameType), currentPlayer('X'), board(SIZE, std::string(SIZE, '.')) {
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

    std::string printBoard() {
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

    bool invalidMove(std::pair <int, int> &move) {
        return (move.first >= SIZE || move.second >= SIZE || board[move.first][move.second] != '.');
    }
    bool    notStartedYet() {
        return (SIZE == 0);
    }
    std::pair<int, int>    playEasy() {
        std::vector<std::pair<int, int> > possibleMoves = getPossibleMoves();
        std::pair <int, int> move = possibleMoves[rand() % possibleMoves.size()];
        return move;
    }
    
    std::pair<int, int> playMedium() {
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

    char getWinner() {
        return winner;
    }

    void    checkTicTacToeEnd(std::pair<int, int> &move) {
        
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

    bool validIndexes(int x, int y) {
        return (x >= 0 && y >= 0 && x < SIZE && y < SIZE);
    }
    int countConsecutiveCells(std::pair<int, int> &p, char player) {
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

    void    checkGomukoEnd(std::pair<int, int> &move) {
        movesLeft--;
        static const int winCondition = 5;

        if (countConsecutiveCells(move, currentPlayer) >= winCondition) {
            gameEnd = 1;
            winner = currentPlayer;
        }
        else if (!movesLeft) gameEnd = 1;
    }

    void    playMove(std::pair <int, int> &move) {
        board[move.first][move.second] = currentPlayer;
        if (gameType == "TICTACTOE")
            checkTicTacToeEnd(move);
        else checkGomukoEnd(move);
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }

    bool isTerminal() {
        return gameEnd;
    }

    char getCurrentPlayer() {
        return currentPlayer;
    }

    std::vector<std::pair<int, int> > getPossibleMoves() {
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
};

struct Node {
    Game state;
    Node *parent;
    std::pair<int, int> move;
    int visitCount;
    double winScore;
    std::vector<Node*> children;
    std::vector<std::pair<int, int> > expandableMoves;
    Node(const Game& state, Node* parent = nullptr, std::pair <int, int> mv = std::make_pair(-1, -1))
        : state(state), parent(parent), move(mv), visitCount(0), winScore(0){
            expandableMoves = this->state.getPossibleMoves();
        }

    ~Node() {
        for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); it++) {
            delete *it;
        }
    }

    bool isFullyExpanded() {
        return expandableMoves.empty() && children.size() > 0;
    }

   
    Node* selectPromisingNode() {
        Node* bestNode = NULL;
        double bestValue = -1e9;
        for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); it++) {
            Node *child = *it;
            double uctValue = child->winScore / child->visitCount + 1.41 * sqrt(log(visitCount) / child->visitCount);
            if (bestValue < uctValue) {
                bestValue = uctValue;
                bestNode = child;
            }
        }
        return bestNode;
    }
    Node* expandNode() {
        int i = rand() % expandableMoves.size();
        Game newState = state;
        newState.playMove(expandableMoves[i]);
        Node* newNode = new Node(newState, this, expandableMoves[i]);
        swap(expandableMoves[i], expandableMoves.back());
        expandableMoves.pop_back();
        children.push_back(newNode);
        return newNode;
    }
    void backpropagate(char winner) {
        visitCount++;
        // cerr << winner << ' ' << state.getCurrentPlayer() << "::" << endl;
        if (winner == 'N')  winScore += 0.4;
        else if (state.getCurrentPlayer() != winner) winScore += 1.0;
        // else winScore += -0.5;
        if (parent) parent->backpropagate(winner);
    }
    char    simulateMove() {
        Game tmp = state;
        std::vector <std::pair<int, int> > possibleMoves = tmp.getPossibleMoves();
        while (!tmp.isTerminal()) {
            int i = rand() % possibleMoves.size();
            tmp.playMove(possibleMoves[i]);
            swap(possibleMoves[i], possibleMoves.back());
            possibleMoves.pop_back();
        }
        return tmp.getWinner();
    }
};

class MCTS {
    int iterations;
    std::default_random_engine generator;
public:
    MCTS(int iterations) : iterations(iterations), generator(time(0)) {}

    std::pair<int, int> findNextMove(Game& game) {
        Node* root = new Node(game);
        for (int i = 0; i < iterations; i++) {
            Node *it = root;
            while (it->isFullyExpanded())
                it = it->selectPromisingNode();
            if (!it->state.isTerminal()) {
                it = it->expandNode();
            }
            char result = it->simulateMove();
            it->backpropagate(result);
        }
        std::map<std::pair<int, int>, double> moves;
        double sum = 0;
        double bestVal = -1e9;
        std::pair<int, int> bestMove = std::make_pair(-1, -1);
        // cerr << root->children.size() << endl;
        for (std::vector<Node*>::iterator it = root->children.begin(); it != root->children.end(); it++) {
            Node *child = *it;
            moves[child->move] += child->visitCount;
            sum += child->visitCount;
        }
        for(std::map<std::pair<int, int>, double>::iterator it = moves.begin(); it != moves.end(); it++) {
            if (it->second / sum > bestVal) {
                bestVal = it->second / sum;
                bestMove = it->first;
            }
        }
        delete root;
        return bestMove;
    }
};


class Bot {
    private :
        Game ticTacToe;
        Game gomuko;
        std::string ticTacToeDifficulty;
        std::string gomukoDifficulty;
        std::string msgToSend;
        
        bool isInvalidBoardSize(std::string &num, std::string &gameType) {
            bool invalid = (num.size() > 2);
            int boardSize = 0;
            for(size_t i = 0; i < num.size(); i++) {
                invalid |= !std::isdigit(num[i]);
                boardSize = boardSize * 10 + num[i] - '0';
            }
            invalid |= ((boardSize > 10 || boardSize < 3));
            if (gameType == "GOMUKO") invalid |= (boardSize < 5);
            return invalid; 
        }

        bool    isInvalidCoordinates(std::string &X, std::string &Y) {
            bool    invalid = (X.size() > 2 || Y.size() > 2);
            int x = 0, y = 0;
            for(size_t i = 0; i < X.size(); i++) {
                invalid |= !std::isdigit(X[i]);
                x = x * 10 + X[i] - '0';
            }
            for(size_t i = 0; i < Y.size(); i++) {
                invalid |= !std::isdigit(Y[i]);
                y = y * 10 + Y[i] - '0';
            }
            invalid |= (y < 0 || y > 10 || x < 0 || x > 10);
            return invalid;
        }

        bool    isInvalidDifficulty(std::string &gameDifficulty) {
            return (gameDifficulty != "HARD" && gameDifficulty != "MEDIUM" && gameDifficulty != "EASY");
        }

        bool isInvalidGameType(std::string &gameType) {
            return (gameType != "GOMUKO" && gameType != "TICTACTOE");
        }

        bool    isValidMove(std::string gameType, std::pair<int, int> move) {
            std::vector<std::pair<int, int> > moves;
            moves = (gameType == "TICTACTOE") ? ticTacToe.getPossibleMoves() : gomuko.getPossibleMoves();
            return  (std::find(moves.begin(), moves.end(), move) != moves.end());
        }
        
        bool    isTerminal(std::string gameType) {
            return ((gameType == "TICTACTOE") ? ticTacToe.isTerminal() : gomuko.isTerminal());
        }

        bool isGameFinished(Game &game, bool needPrint) {
            if (!game.isTerminal()) return 0;
            if (needPrint) msgToSend += game.printBoard();
            if (game.getWinner() == 'N') msgToSend += "Game ended with a DRAW, ";
            else  {
                msgToSend += ((game.getWinner() == 'X') ? "Bot" : "You");
                msgToSend += " have won, ";
            }
            msgToSend += "please start a new game if you want to play\n";
            return 1;
        }
        void    startNewGame(std::string gameType, std::string gameDifficulty, int boardSize) {
            if (gameType == "TICTACTOE") {
                ticTacToe = Game(boardSize, gameType);
                ticTacToeDifficulty = gameDifficulty;
            }
            else {
                gomuko = Game(boardSize, gameType);
                gomukoDifficulty = gameDifficulty;
            }
            msgToSend.clear();
            playServerMove(gameType);
        }

        void    playServerMove(std::string gameType) {
            Game &game = gameType == "GOMUKO" ? gomuko : ticTacToe;
            std::string difficulty = gameType == "GOMUKO" ? gomukoDifficulty : ticTacToeDifficulty;
            std::pair<int, int> botMove;
            if (difficulty == "EASY") 
                botMove = game.playEasy();
            else if (difficulty == "MEDIUM")
                botMove = game.playMedium();
            else {
                MCTS mct(5000);
                botMove = mct.findNextMove(game);
            }
            game.playMove(botMove);
            msgToSend += game.printBoard();
            // sendMoveToClient(botMove, gameType);
            msgToSend += "Bot Move : [" + std::to_string(botMove.first) + ' ' + std::to_string(botMove.second) + "]\n";
            if (isGameFinished(game, 0))
                return ;
            else msgToSend += "Your turn to play, please chose an action : [x, y]\n";
        }

        void    playClientMove(std::string gameType, std::pair <int, int> clientMove) {
            Game &game = gameType == "GOMUKO" ? gomuko : ticTacToe;
            if (game.isTerminal()) {
                msgToSend = gameType + " Game have finished, please start a new one to play\n";
                return ;
            }
            else if (game.notStartedYet()) {
                msgToSend = gameType + " game didn't start yet, please start a new Game to play\n";
                return ;
            }
            else if (game.invalidMove(clientMove)) {
                msgToSend = "Invalid Move, please try again\n";
                return ;
            }
            game.playMove(clientMove);
            msgToSend = "Client Move : [" + std::to_string(clientMove.first) + ' ' + std::to_string(clientMove.second) + "]\n";
       
            if (isGameFinished(game, 1)) return ;
            else playServerMove(gameType);
        }

    public :
        Bot() {}

        std::string play(std::vector<std::string> &cmd) {
            if (cmd[1] == "START")
                startNewGame(cmd[2], cmd[3], std::stoi(cmd[4]));
            else
                playClientMove(cmd[2], std::make_pair(stoi(cmd[3]), stoi(cmd[4])));
            return msgToSend;
        }
       
        std::string botUsage() {
            std::ostringstream oss;
            oss << "Invalid Command, please follow the command prototype below:\n";
            oss << "Bot Usage :\n";
            oss << "    1. BOT START GAME DIFFICULTY SIZE\n";
            oss << "        - GAME: 'GOMUKO' or 'TICTACTOE'\n";
            oss << "        - DIFFICULTY: 'HARD', 'MEDIUM', 'EASY'\n";
            oss << "        - SIZE: 3-10 for TICTACTOE, 5-10 for GOMUKO\n";
            oss << "    2. BOT PLAY GAME X Y\n";
            oss << "        - X, Y: Coordinates for the move (positive integers)\n";
            oss << "        - GAME: 'GOMUKO' or 'TICTACTOE'\n";

            return oss.str();
        }
        
        bool argumentsError(std::vector<std::string> &cmd) {
            bool isError = (cmd.size() != 5) || (cmd[1] != "PLAY" && cmd[1] != "START") || isInvalidGameType(cmd[2]);
            if (isError) return isError;

            if (cmd[1] == "START")
                isError = (isInvalidDifficulty(cmd[3]) || isInvalidBoardSize(cmd[4], cmd[2]));
            else
                isError = isInvalidCoordinates(cmd[3], cmd[4]);
            return isError;
        }
        ~Bot() {};
};