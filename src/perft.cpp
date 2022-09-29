#include "game.h"
#include <chrono>
#include <algorithm> 

template<Color color>
uint64_t search(int depth, Chess *game) {
    if (depth == 0) {
        return 1;
    }

    std::vector<Move> moves = game->getMoves<color>();

    uint64_t positions = 0;

    for (Move move : moves) {
        game->makeMove<color>(move);
        positions += search<~color>(depth-1, game);
        game->unmakeMove<color>(move);
    }

    return positions;
}

template<Color color>
uint64_t search(int depth, Chess game) {
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

int main(int argc, char * argv[]) {
    Chess game;
    Color color = WHITE;
    unsigned int depth = -1;
    
    if (argc == 2) { //Given a fen
        color = game.setFen(argv[1]);
    } else if (argc == 3) { //Given a fen and a depth
        color = game.setFen(argv[1]);
        depth = std::stoi(std::string(argv[2]));
    }

    game.print();

    for (int i = 0; i <= depth; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        auto positions = color == WHITE ? search<WHITE>(i, game) : search<BLACK>(i, game);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> sec = end - start;
        std::cout << std::endl << "Depth: " << i << std::endl;
        std::cout << "Nodes: " << positions << std::endl;
        std::cout << std::fixed << (positions / (sec.count() / 1000.0)) << " nps\n" << std::endl;
    }
}