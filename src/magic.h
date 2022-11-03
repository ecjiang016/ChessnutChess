#pragma once

#include "bits.h"
#include "masks.h"
#include "piece.h"

namespace Magic {
    constexpr int nbits = 10;
    extern const Bitboard bishop_magics[64];
    extern const Bitboard rook_magics[64];
    const Bitboard rook_attacks[64];
    const Bitboard bishop_attacks[64];

    const uint8_t bishop_shifts[64] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    const uint8_t rook_shifts[64] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    template<PieceType = Rook>
    inline Bitboard get_index(Bitboard bb, int piece_loc) {
        return (bb * rook_magics[piece_loc]) >> rook_shifts[piece_loc];
    }

    template<>
    inline Bitboard get_index<Bishop>(Bitboard bb, int piece_loc) {
        return (bb * bishop_magics[piece_loc]) >> bishop_magics[piece_loc];
    }

    void computeMagics();
    void initializeTables();
}

inline void sliding_moves_rook(Bitboard occupancy, int piece_loc) {
    Magic::get_index(occupancy & rook_masks[piece_loc], piece_loc);
}