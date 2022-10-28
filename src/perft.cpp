#include "perft.h"
#include <chrono>
#include <algorithm> 

template<Color color>
uint64_t search(int depth, Chess *game) {
    if (depth == 0) {
        return 1;
    }

    MoveArray moves = game->getMoves<color>();

    uint64_t positions = 0;

    for (Move move : moves) {
        game->makeMove<color>(move);
        positions += search<~color>(depth-1, game);
        game->unmakeMove<color>(move);
    }

    return positions;
}

template<Color color>
uint64_t search(int depth, Chess game, bool extra_info = true) {
    if (depth == 0) {
        return 1;
    }

    uint64_t positions = 0;
    std::vector<std::string> moves_in_pos;

    for (Move move : game.getMoves<color>()) {
        game.makeMove<color>(move);
        uint64_t move_positions = search<~color>(depth-1, &game);
        game.unmakeMove<color>(move);
        
        positions += move_positions;
        moves_in_pos.push_back(move.UCI() + ": " + std::to_string(move_positions));
    }
    std::sort(moves_in_pos.begin(), moves_in_pos.end());
    for (std::string pos : moves_in_pos) {
        std::cout << pos << std::endl;
    }

    return positions;
}

void perft(Chess game, Color color, unsigned int depth) {
    bool single_count = false;

    if (!single_count) {
        game.print();
        for (int i = 0; i <= depth; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            auto positions = color == WHITE ? search<WHITE>(i, game) : search<BLACK>(i, game);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> sec = end - start;
            std::cout << '\n';
            std::cout << std::fixed << (positions / (sec.count() / 1000.0)) << " nps\n";
            std::cout << "Depth: " << i << '\n';
            std::cout << "Nodes: " << positions << '\n' << std::endl;
        }
    } else {
        //Uses the search which takes in a pointer so it avoids the debugging prints
        auto positions = color == WHITE ? search<WHITE>(depth, &game) : search<BLACK>(depth, &game);
        std::cout << positions << std::endl;
    }
}