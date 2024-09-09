#ifndef MCTS_HPP
#define MCTS_HPP
#include "Game.hpp"

struct Node {
    Game state;
    Node *parent;
    std::pair<int, int> move;
    int visitCount;
    double winScore;
    std::vector<Node*> children;
    std::vector<std::pair<int, int> > expandableMoves;
    Node(const Game& state, Node* parent = NULL, std::pair <int, int> mv = std::make_pair(-1, -1));
    ~Node();
    bool    isFullyExpanded();
    Node*   selectPromisingNode();
    Node*   expandNode();
    void    backpropagate(char winner);
    char    simulateMove();
};

struct MCTS {
    int iterations;
    MCTS(int iterations);
    std::pair<int, int> findNextMove(Game& game);
};

#endif