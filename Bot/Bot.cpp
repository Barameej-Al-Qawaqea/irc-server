#include "Bot.hpp"

bool Bot::isInvalidBoardSize(std::string &num, std::string &gameType) {
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

bool    Bot::isInvalidCoordinates(std::string &X, std::string &Y) {
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

bool    Bot::isInvalidDifficulty(std::string &gameDifficulty) {
    return (gameDifficulty != "HARD" && gameDifficulty != "MEDIUM" && gameDifficulty != "EASY");
}

bool    Bot::isInvalidGameType(std::string &gameType) {
    return (gameType != "GOMUKO" && gameType != "TICTACTOE");
}

bool      Bot::isInvalidLogin(std::string &login) {
    long long num = 0;
    for (size_t i = 0; i < login.size(); i++) {
        if (!std::isdigit(login[i])) return true;
        num = num * 10 + login[i] - '0';
        if (num > INT_MAX) return true;
    }
    return false;
}

bool    Bot::isValidMove(std::string gameType, std::pair<int, int> move) const {
    std::vector<std::pair<int, int> > moves;
    moves = (gameType == "TICTACTOE") ? ticTacToe.getPossibleMoves() : gomuko.getPossibleMoves();
    return  (std::find(moves.begin(), moves.end(), move) != moves.end());
}

bool    Bot::isTerminal(std::string gameType) const {
    return ((gameType == "TICTACTOE") ? ticTacToe.isTerminal() : gomuko.isTerminal());
}

bool Bot::isGameFinished(Game &game, bool needPrint) {
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

void    Bot::startNewGame(std::string gameType, std::string gameDifficulty, int boardSize) {
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

void    Bot::playServerMove(std::string gameType) {
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
    std::ostringstream oss;
    oss << "Bot Move : [" << botMove.first << ' ' << botMove.second << "]\n";
    msgToSend += oss.str();
    if (isGameFinished(game, 0))
        return ;
    else msgToSend += "Your turn to play, please chose an action : [x, y]\n";
}

void    Bot::playClientMove(std::string gameType, std::pair <int, int> clientMove) {
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
    std::ostringstream oss;
    oss << "Client Move : [" << clientMove.first << ' ' << clientMove.second << "]\n";
    msgToSend = oss.str();

    if (isGameFinished(game, 1)) return ;
    else playServerMove(gameType);
}

Bot::Bot() {}

std::string Bot::play(std::vector<std::string> &cmd) {
    if (cmd[0] == "START")
        startNewGame(cmd[1], cmd[2], std::atoi(cmd[3].c_str()));
    else
        playClientMove(cmd[1], std::make_pair(atoi(cmd[2].c_str()), std::atoi(cmd[3].c_str())));
    return msgToSend;
}

std::string Bot::botUsage()  {
    std::ostringstream oss;
    oss << "Invalid Command, please follow the command prototype below:\n";
    oss << "Bot Usage :\n";
    oss << "    1. SETUP LOGIN\n";
    oss << "        -LOGIN is a positive integer";
    oss << "    2. START GAME DIFFICULTY SIZE LOGIN\n";
    oss << "        - GAME: 'GOMUKO' or 'TICTACTOE'\n";
    oss << "        - DIFFICULTY: 'HARD', 'MEDIUM', 'EASY'\n";
    oss << "        - SIZE: 3-10 for TICTACTOE, 5-10 for GOMUKO\n";
    oss << "    3. BOT PLAY GAME X Y LOGIN\n";
    oss << "        - X, Y: Coordinates for the move (positive integers)\n";
    oss << "        - GAME: 'GOMUKO' or 'TICTACTOE'\n";

    return oss.str();
}


bool    Bot::argumentsError(std::vector<std::string> &cmd)  {
    bool isError = (cmd.size() != 5 && cmd.size() != 2);
    if (cmd.size() == 5)
        isError |= (cmd[0] != "PLAY" && cmd[0] != "START") || isInvalidGameType(cmd[1]) || isInvalidLogin(cmd.back());
    if (cmd.size() == 2)
        isError |= cmd[0] != "SETUP" || isInvalidLogin(cmd.back());
    if (isError || cmd.size() == 2) return isError;

    if (cmd[1] == "START")
        isError = (isInvalidDifficulty(cmd[3]) || isInvalidBoardSize(cmd[4], cmd[2]));
    else
        isError = isInvalidCoordinates(cmd[3], cmd[4]);
    return isError;
}

Bot::~Bot() {};

