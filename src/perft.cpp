#include "game/game.cpp"
#include <chrono>
#include <algorithm> 

int search_captures(int depth, Chess game, Move move) {
    game.makeMove(move);

    if (depth == 0) {
        if (move.capture != None) {
            return 1;
        } else {
            return 0;
        }
    }

    int checks = 0;

    for (Move move : game.allMoves()) {
        checks += search_captures(depth-1, game, move);
    }

    return checks;
}

int search_captures(int depth, Chess game) {
    if (depth == 0) {
        return 0;
    }

    int checks = 0;

    for (Move move : game.allMoves()) {
        checks += search_captures(depth-1, game, move);
    }

    return checks;
}

int search_checks(int depth, Chess game, Move move) {
    game.makeMove(move);

    if (depth == 0) {
        if (game.inCheck()) {
            return 1;
        } else {
            return 0;
        }
    }

    int checks = 0;

    for (Move move : game.allMoves()) {
        checks += search_checks(depth-1, game, move);
    }

    return checks;
}

int search_checks(int depth, Chess game) {
    if (depth == 0) {
        if (game.inCheck()) {
            return 1;
        } else {
            return 0;
        }
    }

    int checks = 0;

    for (Move move : game.allMoves()) {
        checks += search_checks(depth-1, game, move);
    }

    return checks;
}

int search(int depth, Chess game, Move move) {
    game.makeMove(move);

    std::vector<Move> moves = game.allMoves();

    if (depth == 0) {
        return 1;
    }

    int positions = 0;

    for (Move move : moves) {
        positions += search(depth-1, game, move);
    }

    return positions;
}

int search(int depth, Chess game) {
    if (depth == 0) {
        return 1;
    }

    int positions = 0;
    std::vector<std::string> moves_in_pos;

    for (Move move : game.allMoves()) {
        int move_positions = search(depth-1, game, move);
        positions += move_positions;
        moves_in_pos.push_back(move.UCI() + ": " + std::to_string(move_positions));
    }
    std::sort(moves_in_pos.begin(), moves_in_pos.end());
    for (std::string pos : moves_in_pos) {
        std::cout << pos << std::endl;
    }

    return positions;
}

int main() {
    Chess game;
    //game.loadFen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1");
    //Move fm("d2d3", Pawn);
    //game.makeMove(fm);

    for (int i = 0; i <= 8; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        int positions = search(i, game);
        int checks = search_checks(i, game);
        int captures = search_captures(i, game);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> sec = end - start;
        std::cout << std::endl << "Depth: " << i << std::endl;
        std::cout << "Nodes: " << positions << std::endl;
        std::cout << "Checks: " << checks << std::endl;
        std::cout << "Captures: " << captures << std::endl;
        std::cout << std::fixed << (positions / (sec.count() / 1000.0)) * 3 << " nps" << std::endl << std::endl;
    }
}