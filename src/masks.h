#pragma once
#include <stdint.h>

typedef uint64_t Bitboard;

const Bitboard TOP_ROW = 0xFF00000000000000;
const Bitboard BOTTOM_ROW = 0xFF;
const Bitboard RIGHT_COLUMN = 0x8080808080808080;
const Bitboard LEFT_COLUMN = 0x101010101010101;
const Bitboard TOP_TWO = 0xFFFF000000000000;
const Bitboard BOTTOM_TWO = 0xFFFF;
const Bitboard RIGHT_TWO = 0xC0C0C0C0C0C0C0C0;
const Bitboard LEFT_TWO= 0x303030303030303;

extern const Bitboard rook_masks_horizontal[64];
extern const Bitboard rook_masks_vertical[64];
extern const Bitboard bishop_masks_diag1[64];
extern const Bitboard bishop_masks_diag2[64];
extern const Bitboard knight_masks[64];
extern const Bitboard king_masks[64];
extern const Bitboard all_direction_masks[64];
extern const Bitboard directional_masks[64][8];
extern const Bitboard connecting_masks[64][64];
extern const Bitboard ray_masks[64][64];

template<Color color, Flag flag>
constexpr Bitboard castling_pieces() {
    return color == WHITE ? (flag == CASTLE_SHORT ? Bitboard(0b10010000) : Bitboard(0b00010001)) :
        (flag == CASTLE_SHORT ? Bitboard(0b10010000) << 56 : Bitboard(0b00010001) << 56);
}

template<Color Color, Flag flag>
constexpr Bitboard king_castle_spaces() {
    return color == WHITE ? (flag == CASTLE_SHORT ? Bitboard(0b01100000) : Bitboard(0b00001100)) :
        (flag == CASTLE_SHORT ? Bitboard(0b01100000) << 56 : Bitboard(0b00001100) << 56);
}