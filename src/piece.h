#pragma once
#include "bits.h"
#include "masks.h"
#include <vector>

extern const std::string index_to_string[64];

enum Color : uint8_t {
    WHITE = 0, BLACK = 1
};

constexpr Color operator~(Color color) {
    return Color(color ^ Color(1));
}

enum PieceType : uint8_t {
    NoPieceType = 0,
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

inline PieceType getPieceType(Piece piece) {
    return PieceType(piece & 0b111);
}

inline Color getPieceColor(Piece piece) {
    return Color((piece >> 3) & 0b11);
}

enum Flag : uint16_t {
    QUIET   =  0b1000000000000,
    CAPTURE = 0b10000000000000,
    DOUBLE_PUSH = 0b11000000000000,
    EN_PASSANT = 0b100000000000000
};

struct Move {
  private:
    // | flag |   old_c   |   new_c   |
    // 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
    
    uint16_t move;

  public:
    inline Move() : move(0) {}

    inline Move(uint8_t from, uint8_t to) {
        move = QUIET | (from << 6) | to;
    }

    inline Move(int from, int to, Flag flag) {
        move = flag | (from << 6) | to;
    }

    inline uint8_t from() {
        return (move >> 6) & 0b111111;
    }

    inline uint8_t to() {
        return move & 0b111111;
    }

    inline Flag flag() {
        return Flag(move & 0b1111000000000000);
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
        return ((pawns & ~LEFT_COLUMN) << 7) | ((pawns & ~RIGHT_COLUMN) << 9);
    } else {
        return ((pawns & ~LEFT_COLUMN) >> 9) | ((pawns & ~RIGHT_COLUMN) >> 7);
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

template<Flag flag = QUIET>
inline void add_moves(uint8_t piece_pos, Bitboard move_bitboard, std::vector<Move> &moves) {
    while (move_bitboard) {
        moves.push_back(Move(piece_pos, bitScanForward(move_bitboard), flag));
        move_bitboard &= move_bitboard - 1;
    }
}

std::ostream &operator<<(std::ostream &out, Move move);