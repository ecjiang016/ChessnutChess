#include "game.h"
#include <chrono>
#include <algorithm> 

template<Color color>
int search(int depth, Chess game, Move move) {
    if (depth == 0) {
        return 1;
    }

    game.makeMove<color>(move);
    std::vector<Move> moves = game.getMoves<color>();

    int positions = 0;

    for (Move move : moves) {
        positions += search<~color>(depth-1, game, move);
    }

    return positions;
}

template<Color color>
int search(int depth, Chess game) {
    if (depth == 0) {
        return 1;
    }

    int positions = 0;
    std::vector<std::string> moves_in_pos;

    for (Move move : game.getMoves<color>()) {
        int move_positions = search<~color>(depth-1, game, move);
        positions += move_positions;
        moves_in_pos.push_back(move.UCI() + ": " + std::to_string(move_positions));
    }
    std::sort(moves_in_pos.begin(), moves_in_pos.end());
    for (std::string pos : moves_in_pos) {
        std::cout << pos << std::endl;
    }

    return positions;
}

int main(int argc, char * argv[]) {
    Chess game;
    game.setFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    std::cout << game.getFen() << std::endl;
    game.print();

    int depth = 4;
    if (argc == 2) {
        std::string depth_str = argv[1];
        depth = std::stoi(depth_str);
    }

    for (int i = 0; i <= depth; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        int positions = search<WHITE>(i, game);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> sec = end - start;
        std::cout << std::endl << "Depth: " << i << std::endl;
        std::cout << "Nodes: " << positions << std::endl;
        std::cout << std::fixed << (positions / (sec.count() / 1000.0)) << " nps" << std::endl << std::endl;
    }
}