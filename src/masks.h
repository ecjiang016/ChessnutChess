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

extern const Bitboard rook_masks[64];
extern const Bitboard rook_masks_edge[64];
extern const Bitboard bishop_masks[64];
extern const Bitboard bishop_masks_edge[64];
extern const Bitboard knight_masks[64];
extern const Bitboard king_masks[64];
extern const Bitboard all_direction_masks[64];
extern const Bitboard directional_masks[64][8];
extern const Bitboard connecting_masks[64][64];
extern const Bitboard ray_masks[64][64];

enum Color : uint8_t;

template<Color color>
constexpr Bitboard promotion_row() {
    return color == Color(0) ? TOP_ROW : BOTTOM_ROW;
}