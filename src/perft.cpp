#include "perft.h"
#include <chrono>
#include <algorithm> 

template<class GameState state>
uint64_t search(int depth, Chess *game) {
    if (depth == 0) {
        return 1;
    }

    MoveArray moves = game->getMoves<state>();

    uint64_t positions = 0;

    for (Move move : moves) {
        game->makeMove<state>(move);

        switch (move.flag()) {
            case DOUBLE_PUSH:
                positions += search<state.next<DOUBLE_PUSH>()>(depth-1, game);
                break;
            default:
                positions += search<state.next<QUIET>()>(depth-1, game);
                break;
        }

        game->unmakeMove<state>(move);
    }

    return positions;
}

template<class GameState state>
uint64_t search(int depth, Chess game, bool extra_info = true) {
    if (depth == 0) {
        return 1;
    }

    uint64_t move_positions;
    uint64_t positions = 0;
    std::vector<std::string> moves_in_pos;

    for (Move move : game.getMoves<state>()) {
        game.makeMove<state>(move);
        switch (move.flag()) {
            case DOUBLE_PUSH:
                move_positions = search<state.next<DOUBLE_PUSH>()>(depth-1, &game);
                break;
            default:
                move_positions = search<state.next<QUIET>()>(depth-1, &game);
                break;
        }
        game.unmakeMove<state>(move);
        
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
            auto positions = color == WHITE ? search<GameState(WHITE, false)>(i, game) : search<GameState(BLACK, false)>(i, game);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> sec = end - start;
            std::cout << '\n';
            std::cout << std::fixed << (positions / (sec.count() / 1000.0)) << " nps\n";
            std::cout << "Depth: " << i << '\n';
            std::cout << "Nodes: " << positions << '\n' << std::endl;
        }
    } else {
        //Uses the search which takes in a pointer so it avoids the debugging prints
        auto positions = color == WHITE ? search<GameState(WHITE, false)>(depth, &game) : search<GameState(BLACK, false)>(depth, &game);
        std::cout << positions << std::endl;
    }
}