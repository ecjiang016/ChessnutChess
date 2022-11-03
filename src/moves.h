#pragma once
#include "bits.h"
#include "piece.h"
#include "masks.h"

extern const std::string index_to_string[64];

inline int string_to_index(std::string str) {
    return ((str[1] - '1') * 8) + (str[0] - 'a');
}

enum Flag : uint16_t {
    QUIET        =  0b001000000000000,
    CAPTURE      =  0b010000000000000,
    DOUBLE_PUSH  =  0b011000000000000,
    EN_PASSANT   =  0b100000000000000,
    CASTLE_SHORT =  0b101000000000000,
    CASTLE_LONG  =  0b110000000000000,
    PROMOTION_CAPTURE =  0b111000000000000, //This element not used for Move struct flag but rather for templates that take flags
    PROMOTION_KNIGHT  = 0b1000000000000000,
    PROMOTION_BISHOP  = 0b1001000000000000,
    PROMOTION_ROOK    = 0b1010000000000000,
    PROMOTION_QUEEN   = 0b1011000000000000,
    PROMOTION_CAPTURE_KNIGHT = 0b1100000000000000,
    PROMOTION_CAPTURE_BISHOP = 0b1101000000000000,
    PROMOTION_CAPTURE_ROOK   = 0b1110000000000000,
    PROMOTION_CAPTURE_QUEEN  = 0b1111000000000000
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

    Move(std::string uci) {
        move = QUIET | ((((uci[1] - '1') * 8) + (uci[0] - 'a')) << 6) | ((uci[3] - '1') * 8) + (uci[2] - 'a');
    }

    Move(std::string uci, Flag flag) {
        move = flag | ((((uci[1] - '1') * 8) + (uci[0] - 'a')) << 6) | ((uci[3] - '1') * 8) + (uci[2] - 'a');
    }

    inline uint8_t from() const {
        return (move >> 6) & 0b111111;
    }

    inline uint8_t to() const {
        return move & 0b111111;
    }

    inline Flag flag() const {
        return Flag(move & 0b1111000000000000);
    }

    inline std::string UCI() const {
        //Still needs logic for special moves
        return index_to_string[from()] + index_to_string[to()];
    }

};

constexpr size_t MOVE_VECTOR_SIZE = 256;

template<Color color>
struct MoveArray {
  private:
    Move arr[MOVE_VECTOR_SIZE];
    Move *last;
  public:
    inline const Move* begin() const {
        return arr;
    }

    inline Move* end() const {
        return last;
    }

    inline size_t size() const {
        return last - arr;
    }

    friend class Chess;

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

//Attacks computed with hyperbola quintessence
//Pretty much used to pre-compute moves for the faster magic bitboard move gen
template<PieceType = Bishop>
inline Bitboard HQ_attacks(int pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, bishop_masks_diag1[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, bishop_masks_diag2[pos_idx], get_single_bitboard(pos_idx));
}

template<>
inline Bitboard HQ_attacks<Rook>(int pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, rook_masks_horizontal[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, rook_masks_vertical[pos_idx],   get_single_bitboard(pos_idx));
}

template<PieceType = Knight>
inline Bitboard get_attacks(int pos_idx, Bitboard occupancy) {
    return knight_masks[pos_idx];
}

template<>
inline Bitboard get_attacks<Bishop>(int pos_idx, Bitboard occupancy) {
    return Magic::bishop_attacks[pos_idx][((bishop_masks[pos_idx] & occupancy) * Magic::bishop_magics[pos_idx]) >> Magic::bishop_shifts[pos_idx]];
}

template<>
inline Bitboard get_attacks<Rook>(int pos_idx, Bitboard occupancy) {
    return Magic::rook_attacks[pos_idx][((rook_masks[pos_idx] & occupancy) * Magic::rook_magics[pos_idx]) >> Magic::rook_shifts[pos_idx]];
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
inline void add_moves(uint8_t piece_pos, Bitboard move_bitboard, Move* &moves) {
    while (move_bitboard) {
        *moves++ = Move(piece_pos, bitScanForward(move_bitboard), flag);
        move_bitboard &= move_bitboard - 1;
    }
}

//Used to add all capture promotions
template<>
inline void add_moves<PROMOTION_CAPTURE>(uint8_t piece_pos, Bitboard move_bitboard, Move* &moves) {
    while (move_bitboard) {
        *moves++ = Move(piece_pos, bitScanForward(move_bitboard), PROMOTION_CAPTURE_KNIGHT);
        *moves++ = Move(piece_pos, bitScanForward(move_bitboard), PROMOTION_CAPTURE_BISHOP);
        *moves++ = Move(piece_pos, bitScanForward(move_bitboard), PROMOTION_CAPTURE_ROOK);
        *moves++ = Move(piece_pos, bitScanForward(move_bitboard), PROMOTION_CAPTURE_QUEEN);
        move_bitboard &= move_bitboard - 1;
    }
}

//Masks for castling
template<Color color, Flag flag>
constexpr Bitboard castling_pieces() {
    return color == WHITE ? (flag == CASTLE_SHORT ? Bitboard(0b10010000) : Bitboard(0b00010001)) :
        (flag == CASTLE_SHORT ? Bitboard(0b10010000) << 56 : Bitboard(0b00010001) << 56);
}

template<Color color, Flag flag>
constexpr Bitboard king_castle_spaces() {
    return color == WHITE ? (flag == CASTLE_SHORT ? Bitboard(0b01100000) : Bitboard(0b00001110)) :
        (flag == CASTLE_SHORT ? Bitboard(0b01100000) << 56 : Bitboard(0b00001110) << 56);
}

//Returns the knight on the long castle side
template<Color color>
constexpr Bitboard long_castle_knight() {
    return color == WHITE ? Bitboard(0b00000010) : Bitboard(0b00000010) << 56;
}

std::ostream &operator<<(std::ostream &out, Move move);