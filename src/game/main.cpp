#include "game.cpp"

int main() {
    Bitboards bitboards;
    FEN("8/ppp2p1r/2N2P2/8/8/2R2R1R/7P/7n", bitboards);

    //std::vector<Move> move;

    struct Move moves [256];
    struct Move *move = moves;

    for (int i = 0; i < 10; i++) {
        Move a(12, 23, Pawn);
        *move++ = a;
    }
    std::cout << moves[9].UCI() <<std::endl;
    std::cout << moves[10].UCI() <<std::endl;
    
    //FEN("rnbqkbnr/1p3p1p/8/2p1p3/2PP2p1/p4P2/PP2P1PP/RNBQKBNR", bitboards);

    /*
    print_bitboards(bitboards);
    
    std::vector<Move> moves;
    PawnMoves(bitboards, moves, 1);
    for(Move i : moves) 
        std::cout << i.UCI() << std::endl;
    */
    
}
