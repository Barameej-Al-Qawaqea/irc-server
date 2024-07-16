#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <limits>
#include <random>
#include <ctime>
#include <iomanip>
using namespace std;

class MCTS;
struct Game {
    char currentPlayer;
    std::vector <string> board;
    int SIZE;
    std::string gameType;    // is it tictactoe or gomuko
    bool    gameEnd = 0;
    char winner;
    int movesLeft;
    vector<vector<int>> row;
    vector<vector<int>> col;
    vector<int> leftDiagonal;
    vector<int> rightDiagonal;
public:
    Game() {}
    Game(int SIZE, std::string &gameType) : SIZE(SIZE), gameType(gameType), currentPlayer('X'), board(SIZE, string(SIZE, '.')) {
        movesLeft = SIZE * SIZE;
        winner = 'N';

        if (gameType == "TICTACTOE") {
            row = vector <vector<int>> (SIZE, vector<int> (2));
            col = vector <vector<int>> (SIZE, vector<int> (2));
            leftDiagonal = vector<int>(2);
            rightDiagonal = vector<int>(2);
        }
    }

    void printBoard() {
        std::cout << "   ";
        for(int i = 0; i < SIZE; i++)
            std::cout << setw(3) << i << ' ';
        std::cout << std::endl;
        std::cout << "   ";
        for (int j = 0; j < SIZE; ++j) {
            std::cout << setw(4) << "---";
        }
        std::cout << endl;

        for (int i = 0; i < SIZE; ++i) {
            std::cout << setw(2) << i << " ";  // Row index

            for (int j = 0; j < SIZE; ++j) {
                std::cout << "| " << board[i][j] << " ";
            }
            std::cout << "|" << endl;

        // Print the bottom border for the row
            std::cout << "   ";
            for (int j = 0; j < SIZE; ++j) {
                std::cout << setw(4) << "---";
            }
            std::cout << endl;
        }
    }

    pair<int, int>    playEasy() {
        vector<pair<int, int>> possibleMoves = getPossibleMoves();
        pair <int, int> move = possibleMoves[rand() % possibleMoves.size()];
        playMove(move);
        return move;
    }
    
    char getWinner() {
        return winner;
    }

    void    checkTicTacToeEnd(pair<int, int> &move) {
        
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
    void    checkGomukoEnd(pair<int, int> &move) {
        int player = currentPlayer == 'X' ? 1 : 0;
        movesLeft--;

        static int side1x[4] = {0, -1, -1, 1};   //x
        static int side1y[4] = {1, 0, 1, 1};  //y

        static int side2x[4] = {0, 1, 1, -1};
        static int side2y[4] = {-1, 0, -1, -1};
        static const int winCondition = 5;
        for(int k = 0; k < 4; k++) {
            int count = 0;
            int x = move.first, y = move.second;
            while (count < winCondition && validIndexes(x, y) && board[x][y] == currentPlayer) {
                count++;
                x += side1x[k];
                y += side1y[k];
            }
            x = move.first + side2x[k], y = move.second + side2y[k];
            while (count < winCondition && validIndexes(x, y) && board[x][y] == currentPlayer) {
                count++;
                x += side2x[k];
                y += side2y[k];
            }
            if (count == winCondition) {
                gameEnd = 1;
                winner = currentPlayer;
                return ;
            }
        }
        if (!movesLeft) gameEnd = 1;
    }

    void    playMove(pair <int, int> &move) {
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

    vector<pair<int, int>> getPossibleMoves() {
        vector<pair<int, int>> possibleMoves;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == '.') {
                    possibleMoves.push_back({i, j});
                }
            }
        }
        return possibleMoves;
    }
};

struct Node {
    Game state;
    Node *parent;
    int visitCount;
    double winScore;
    pair<int, int> move;
    vector<Node*> children;
    vector<pair<int, int>> expandableMoves;
    Node(const Game& state, Node* parent = nullptr, pair<int, int> mv = {-1, -1})
        : state(state), parent(parent), move(mv), visitCount(0), winScore(0){
            expandableMoves = this->state.getPossibleMoves();
        }

    ~Node() {
        for (auto child : children) {
            delete child;
        }
    }

    bool isFullyExpanded() {
        return expandableMoves.empty() && children.size() > 0;
    }

   
    Node* selectPromisingNode() {
        Node* bestNode = nullptr;
        double bestValue = -std::numeric_limits<double>::infinity();
        for (auto &child : children) {
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
        vector <pair<int, int>> possibleMoves = tmp.getPossibleMoves();
        while (!tmp.isTerminal()) {
            int i = rand() % possibleMoves.size();
            tmp.playMove(possibleMoves[i]);
            swap(possibleMoves[i], possibleMoves.back());
            possibleMoves.pop_back();
        }
        char winner = tmp.getWinner();
        return winner;
    }
};

class MCTS {
    int iterations;
    std::default_random_engine generator;
public:
    MCTS(int iterations) : iterations(iterations), generator(time(0)) {}

    pair<int, int> findNextMove(Game& game) {
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
        map<pair<int, int>, double> moves;
        double sum = 0;
        double bestVal = -1e9;
        pair<int, int> bestMove = {-1, -1};
        cerr << root->children.size() << endl;
        for (auto &child : root->children) {
            moves[child->move] += child->visitCount;
            sum += child->visitCount;
            // cerr << child->move.first << ' ' << child->move.second << ' ' << child->winScore << endl;
        }
        for(auto &[l, r] : moves) {
            if (r / sum > bestVal) {
                bestVal = r / sum;
                bestMove = l;
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
    public :
        Bot() {}
        void    startNewGame(std::string gameType, std::string gameDifficulty, int boardSize) {
            if (gameType == "TICTACTOE") {
                ticTacToe = Game(boardSize, gameType);
                ticTacToeDifficulty = gameDifficulty;
            }
            else {
                gomuko = Game(boardSize, gameType);
                gomukoDifficulty = gameDifficulty;
            }
            playServerMove(gameType);
        }

        void    playServerMove(std::string gameType) {
            Game &game = gameType == "GOMUKO" ? gomuko : ticTacToe;
            std::string difficulty = gameType == "GOMUKO" ? gomukoDifficulty : ticTacToeDifficulty;
            pair<int, int> botMove;
            if (difficulty == "EASY") 
                botMove = game.playEasy();
            // else if (difficulty == "MEDIUM")
            //     botMove = game.playMedium();
            else {
                MCTS mct(5000);
                botMove = mct.findNextMove(game);
                game.playMove(botMove);
            }
            // sendMoveToClient(botMove, gameType);
            game.printBoard();
            std::cout << "Bot Move : " << botMove.first << ' ' << botMove.second << endl;
            if (game.isTerminal())
                std::cout << game.getWinner() << ' ' << " wins "<< endl;
            else std::cout << "Your turn to play, chose an action\n";
        }
        void    playClientMove(std::string gameType, pair <int, int> oppMove) {
            Game &game = gameType == "GOMUKO" ? gomuko : ticTacToe;
            game.playMove(oppMove);
            game.printBoard();
            std::cout << "Client Move : " << oppMove.first << ' ' << oppMove.second << endl;
            if (game.isTerminal())
                std::cout << game.getWinner() << ' ' << " wins "<< endl;
        }
        bool    isTerminal(std::string gameType) {
            return ((gameType == "TICTACTOE") ? ticTacToe.isTerminal() : gomuko.isTerminal());
        }
        bool    isValidMove(std::string gameType, pair<int, int> move) {
            vector<pair<int, int>> moves;
            moves = (gameType == "TICTACTOE") ? ticTacToe.getPossibleMoves() : gomuko.getPossibleMoves();
            return  (find(moves.begin(), moves.end(), move) != moves.end()) ;
        }
};

//  START GAME DIFFICULTY SIZE
// PLAY GAME X Y

int main(int ac, char **av) {
    // std::string command = av[1];
    Bot bot;

    std::string gameType = "GOMUKO";     // "you can make it to TICTACTOE or GOMUKO"
    bot.startNewGame(gameType, "HARD", 10);
    int turn = 1;
    while (!bot.isTerminal(gameType)) {
        if (turn & 1) {
            pair <int, int> move;
            std::cin >> move.first >> move.second;
            if (!bot.isValidMove(gameType, move)) {
                std::cout << "Invalid Move, please try again\n";
                continue;
            }
            bot.playClientMove(gameType, move);
        }
        else bot.playServerMove(gameType);
        turn++;
    }
    // bot.startNewGame("")
}