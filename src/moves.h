#pragma once
#include "bits.h"
#include "piece.h"
#include "masks.h"

enum Flag : uint16_t {
    QUIET = 0b1000000000000,
    CAPTURE
};

struct Move {
  private:
    // | flag |   old_c   |   new_c   |
    // 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    
    uint16_t move;

  public:
    Move() {
        move = 0;
    }

    Move(uint8_t old_coord, uint8_t new_coord) {
        move = (old_coord << 6) | new_coord;
    }

    Move(int old_coord, int new_coord, Flag flag) {
        move = (flag << 12) | (old_coord << 6) | new_coord;
    }

    inline uint8_t old_coord() {
        return (move >> 6) & 0b111111;
    }

    inline uint8_t new_coord() {
        return move & 0b111111;
    }

    inline Flag flag() {
        return Flag((move >> 12) & 0b1111);
    }

    inline std::string UCI() {
        //Still needs logic for special moves
        return index_to_string[old_coord()] + index_to_string[new_coord()];
    }

};

inline U64 sliding_moves(U64 occupancy, U64 mask, U64 piece_square_bitboard) {
    return (((occupancy & mask) - piece_square_bitboard) ^
        bswap_64(bswap_64(occupancy & mask) - bswap_64(piece_square_bitboard))) & mask;
}


template<PieceType = Pawn>
uint64_t getAttacks(uint64_t pos_bb, uint64_t occupancy) {
    return 1;
}

template<>
uint64_t getAttacks<Knight>(uint64_t pos_bb, uint64_t occupancy) {
    return knight_masks[bitScanForward(pos_bb)];
}

template<>
uint64_t getAttacks<Bishop>(uint64_t pos_bb, uint64_t occupancy) {
    return sliding_moves(occupancy, bishop_masks_diag1[bitScanForward(pos_bb)], pos_bb) | 
           sliding_moves(occupancy, bishop_masks_diag2[bitScanForward(pos_bb)], pos_bb);
}

template<>
uint64_t getAttacks<Rook>(uint64_t pos_bb, uint64_t occupancy) {
    return sliding_moves(occupancy, rook_masks_horizontal[bitScanForward(pos_bb)], pos_bb) | 
           sliding_moves(occupancy, rook_masks_vertical[bitScanForward(pos_bb)],   pos_bb);
}

template<>
uint64_t getAttacks<Queen>(uint64_t pos_bb, uint64_t occupancy) {
    return getAttacks<Bishop>(pos_bb, occupancy) | getAttacks<Rook>(pos_bb, occupancy);
}

template<>
uint64_t getAttacks<King>(uint64_t pos_bb, uint64_t occupancy) {
    return king_masks[bitScanForward(pos_bb)];
}

