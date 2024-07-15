#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <limits>
#include <random>
#include <ctime>
using namespace std;

class MCTS;
struct Game {
    char currentPlayer;
    vector<string> board;
    int sz;
    std::string gameType;    // is it tictactoe or gomuko
    bool    gameEnd = 0;
    char winner = 'n';
    vector<vector<int>> row, col;
    vector<int> leftDiagonal, rightDiagonal;
    int movesCnt;
public:
    Game() {}
    Game(int sz, std::string &gameType) : sz(sz), gameType(gameType), currentPlayer('x'), board(sz, string(sz, '.')) {
        row = vector <vector<int>> (sz, vector<int> (2));
        col = vector <vector<int>> (sz, vector<int> (2));
        leftDiagonal = vector<int>(2);
        rightDiagonal = vector<int>(2);
        movesCnt = 0;
    }

    void printBoard() {
        for (auto &row : board) {
            for (char j : row) cout << j << ' ';
            cout << endl;
        }
    }

    pair<int, int>    playEasy() {
        vector<pair<int, int>> possibleMoves = getPossibleMoves();
        pair <int, int> move = possibleMoves[rand() % possibleMoves.size()];
        playMove(move);
        return move;
    }
    

    bool checkWin(char player) {
        // Check rows, columns, and diagonals
         map<int, int> row, col;
        int diag1 = 0, diag2 = 0;
        for(int i = 0; i < sz; i++) {
            for(int j = 0; j < sz; j++) {
                if (board[i][j] != player) continue;
                row[i]++;
                col[j]++;
                if (i == j) diag1++;
                if (i == sz - 1 - j) diag2++;
                if (row[i] == sz || col[j] == sz || diag1 == sz || diag2 == sz) return true;
            }
        }
        return false;
    }

    bool checkDraw() {
        for (string &s : board) {
            for (char c : s) if (c == '.') return false;
        }
        return true;
    }

    char getWinner() {
        // if (checkWin('o')) return 'o';
        // if (checkWin('x')) return 'x';
        // return 'n';
        return winner;
    }

    void    playMove(pair<int, int> &move) {
        board[move.first][move.second] = currentPlayer;
        int player = currentPlayer == 'x' ? 1 : 0;
        row[move.first][player] += 1;
        col[move.second][player] += 1;
        if (move.first == move.second)
            leftDiagonal[player]++;
        if (move.first == sz - 1 - move.second)
            rightDiagonal[player]++;
        movesCnt++;
        if (row[move.first][player] == sz || col[move.second][player] == sz
            || leftDiagonal[player] == sz || rightDiagonal[player] == sz) {
            gameEnd = 1;
            winner = currentPlayer;
        }
        else if (movesCnt == sz * sz) gameEnd = 1;
        currentPlayer = (currentPlayer == 'x') ? 'o' : 'x';
    }

    bool isTerminal() {
        return gameEnd;
        // return checkWin('x') || checkWin('o') || checkDraw();
    }

    char getCurrentPlayer() {
        return currentPlayer;
    }

    vector<pair<int, int>> getPossibleMoves() {
        vector<pair<int, int>> possibleMoves;
        for (int i = 0; i < sz; i++) {
            for (int j = 0; j < sz; j++) {
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
        if (winner == 'n')  winScore += 0.4;
        else if (state.getCurrentPlayer() != winner) winScore += 1.0;
        // else winScore += -0.5;
        if (parent) parent->backpropagate(winner);
    }
    char    simulateMove() {
        Game tmp = state;
        vector<pair<int, int>> possibleMoves = tmp.getPossibleMoves();
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
            cerr << child->move.first << ' ' << child->move.second << ' ' << child->winScore << endl;
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
    // void    playTicTacToe() {
    //     if (ticTacToeDifficulty == "EASY")
    //         ticTacToe.playEasy();    // use just random
    //     else if (ticTacToeDifficulty == "MEDIUM")
    //         ticTacToe.playMedium();       // use human logic
    //     else tictTacToe.playHard()   // use comuter simulation IA
    // }
    // void    playGomuko() 
    void    playGomuko() {}
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
                MCTS mct(3000);
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
        bool    terminal() {
            return ticTacToe.isTerminal();
        }
        bool    isValidMove(pair<int, int> move) {
            vector<pair<int, int>> moves = ticTacToe.getPossibleMoves();
            return  (find(moves.begin(), moves.end(), move) != moves.end()) ;
        }
};

//  START GAME DIFFICULTY SIZE
// PLAY GAME X Y

int main(int ac, char **av) {
    // std::string command = av[1];
    Bot bot;
    bot.startNewGame("TICTACTOE", "HARD", 3);
    int turn = 1;
    while(!bot.terminal()) {
        if (turn & 1) {
            pair<int, int> move;
            std::cin >> move.first >> move.second;
            if (!bot.isValidMove(move)) {
                std::cout << "Invalid Move, please try again\n";
                continue;
            }
            bot.playClientMove("TICTACTOE", move);
        }
        else bot.playServerMove("TICTACTOE");
        turn++;
    }
}


// 0110
