#include "game.cpp"

int main() {
    Bitboards bitboards;
    FEN(starting_pos, bitboards);
    print_single_bitboard(bitboards.whitePawn);
    print_single_bitboard(bswap_64(bitboards.whitePawn));
    //FEN("rnbqkbnr/1p3p1p/8/2p1p3/2PP2p1/p4P2/PP2P1PP/RNBQKBNR", bitboards);

    /*
    print_bitboards(bitboards);
    
    std::vector<Move> moves;
    PawnMoves(bitboards, moves, 1);
    for(Move i : moves) 
        std::cout << i.UCI() << std::endl;
    */

    Chess game;
    std::string str_move;
    
    while (1) {
        print_bitboards(game.bitboards);
        std::cin >> str_move;
        Move player_move(str_move, Pawn);
        game.makeMove(player_move);
    }
    
}
