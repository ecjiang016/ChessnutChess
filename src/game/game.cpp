#include "moves.cpp"
#include <cstdlib>
#include "fen.h"

class Chess {
  private:
    bool check;

    void updateCheck() {
        check = color ? bitboards.whiteKing & bitboards.attackBlack : bitboards.blackKing & bitboards.attackWhite;

        //Check for pins
    }

  public:
    bool color;

    Bitboards bitboards;
    Chess() {
        color = 1;
        //FEN(starting_pos, bitboards);
        FEN("2k5/1P3P2/5p2/8/8/8/5R2/1RK5", bitboards);
    }

    void makeMove(Move move) {
        U64* piece_bitboard = bitboards.getBitboard(color, move.piece);
        *piece_bitboard &= ~(C64(1) << move.old_c); //Clear old position
        *piece_bitboard |= C64(1) << move.new_c; //Set piece in the piece's new position
        
        //Clear bit that's being moved to
        for (int p = 1; p <= 6; p++) {
            U64* check_bitboard = bitboards.getBitboard(!color, static_cast<Piece>(p));
            *check_bitboard &= ~(C64(1) << move.new_c); //Clearing that bit in every bitboard
        }

        //Remove captured pawn if en passant
        
        
        //Set en passant board/mask
        bitboards.en_passant = C64(0); //Reset bitboard
        //Set bit if there is a double pawn push
        if ((abs(move.new_c - move.old_c) == 16) && (move.piece == Pawn)) {
            bitboards.en_passant |= C64(1) << move.new_c;
        }



        color = !color;

        updateCheck();
        bitboards.updateOccupancy();
        bitboards.attackBlack = C64(0);
        bitboards.attackWhite = C64(0);
    }

    void flipColor() {
        color = !color;
    }

    void unmakeMove(Move move) {
        
    }

    std::vector<Move> allMoves() {
        std::vector<Move> moves;
        PawnMoves(bitboards, moves, color);
        KnightMoves(bitboards, moves, color);
        BishopMoves(bitboards, moves, color);
        RookMoves(bitboards, moves, color);
        return moves;
    }

};