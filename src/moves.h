#pragma once
#include "bits.h"
#include "masks.h"
#include <vector>

enum Color : uint8_t {
    WHITE = 0, BLACK = 1
};

constexpr Color operator~(Color color) {
    return Color(color ^ Color(1));
}

enum PieceType : uint8_t {
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6
};

enum Piece : uint8_t {
    NoPiece = 0,
    WhitePawn = 1,
    WhiteKnight = 2,
    WhiteBishop = 3,
    WhiteRook = 4,
    WhiteQueen = 5,
    WhiteKing = 6,
    BlackPawn = 9,
    BlackKnight = 10,
    BlackBishop = 11,
    BlackRook = 12,
    BlackQueen = 13,
    BlackKing = 14
};

constexpr Piece makePiece(PieceType piece, Color color) {
    return Piece((color << 3) | piece);
}

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

    Move(uint8_t from, uint8_t to) {
        move = (from << 6) | to;
    }

    Move(int from, int to, Flag flag) {
        move = (flag << 12) | (from << 6) | to;
    }

    inline uint8_t from() {
        return (move >> 6) & 0b111111;
    }

    inline uint8_t to() {
        return move & 0b111111;
    }

    inline Flag flag() {
        return Flag((move >> 12) & 0b1111);
    }

    inline std::string UCI() {
        //Still needs logic for special moves
        return index_to_string[from()] + index_to_string[to()];
    }

};

inline Bitboard sliding_moves(Bitboard occupancy, Bitboard mask, Bitboard piece_square_bitboard) {
    return (((occupancy & mask) - piece_square_bitboard) ^
        bswap_64(bswap_64(occupancy & mask) - bswap_64(piece_square_bitboard))) & mask;
}


template<Color color>
inline Bitboard pawn_attacks(Bitboard pawns) {
    if constexpr (color == WHITE) {
        return ((pawns << 7) & LEFT_COLUMN) | ((pawns << 9) & RIGHT_COLUMN);
    } else {
        return ((pawns >> 9) & LEFT_COLUMN) | ((pawns >> 7) & RIGHT_COLUMN);
    }
}

template<PieceType = Knight>
inline Bitboard get_attacks(int pos_idx, Bitboard occupancy) {
    return knight_masks[pos_idx];
}

template<>
inline Bitboard get_attacks<Bishop>(int pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, bishop_masks_diag1[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, bishop_masks_diag2[pos_idx], get_single_bitboard(pos_idx));
}

template<>
inline Bitboard get_attacks<Rook>(int pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, rook_masks_horizontal[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, rook_masks_vertical[pos_idx],   get_single_bitboard(pos_idx));
}

template<>
inline Bitboard get_attacks<Queen>(int pos_idx, Bitboard occupancy) {
    return get_attacks<Bishop>(pos_idx, occupancy) | get_attacks<Rook>(pos_idx, occupancy);
}

template<>
inline Bitboard get_attacks<King>(int pos_idx, Bitboard occupancy) {
    return king_masks[pos_idx];
}

template<Flag flag>
inline void add_moves(uint8_t piece_pos, Bitboard move_bitboard, std::vector<Move> &moves) {
    while (move_bitboard) {
        moves.push_back(Move(piece_pos, bitScanForward(move_bitboard), flag));
        move_bitboard &= move_bitboard - 1;
    }
}