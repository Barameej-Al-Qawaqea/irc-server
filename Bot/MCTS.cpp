#include "MCTS.hpp"

Node::Node(const Game& state, Node* parent, std::pair <int, int> mv)
    : state(state), parent(parent), move(mv), visitCount(0), winScore(0){
        expandableMoves = this->state.getPossibleMoves();
}

Node::~Node() {
    for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); it++) {
        delete *it;
    }
}

bool    Node::isFullyExpanded() {
    return expandableMoves.empty() && children.size() > 0;
}


Node*   Node::selectPromisingNode() {
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

Node*   Node::expandNode() {
    int i = rand() % expandableMoves.size();
    Game newState = state;
    newState.playMove(expandableMoves[i]);
    Node* newNode = new Node(newState, this, expandableMoves[i]);
    swap(expandableMoves[i], expandableMoves.back());
    expandableMoves.pop_back();
    children.push_back(newNode);
    return newNode;
}

void Node::backpropagate(char winner) {
    visitCount++;
    // cerr << winner << ' ' << state.getCurrentPlayer() << "::" << endl;
    if (winner == 'N')  winScore += 0.4;
    else if (state.getCurrentPlayer() != winner) winScore += 1.0;
    // else winScore += -0.5;
    if (parent) parent->backpropagate(winner);
}

char    Node::simulateMove() {
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

MCTS::MCTS(int iterations) : iterations(iterations) {}
std::pair<int, int> MCTS::findNextMove(Game& game) {
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