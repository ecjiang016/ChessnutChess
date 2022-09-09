#pragma once
#include <stdint.h>
#include <iostream>
#include <string>

typedef uint64_t Bitboard;

inline Bitboard bswap_64(Bitboard bb) {
    bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555);
    bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333);
    bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f);
    bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff);

    return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48);
}

/**
 * Returns false if bit at bit_num is 0, othewise returns true
*/
inline bool check_bit(Bitboard bitboard, int bit_num) {
    return (bitboard & (Bitboard(1) << bit_num)) != 0;
}

inline int pop_count(Bitboard bitboard) {
    int count = 0;
    while (bitboard) {
        count++;
        bitboard &= bitboard - 1; //Remove ls1b
    }
    return count;
}

//Returns a bitboard with the single bit at pos as 1
inline Bitboard get_single_bitboard(uint8_t pos) {
    return Bitboard(1) << pos;
}

void print_bitboard(Bitboard bitboard);

const int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

const uint64_t debruijn64 = uint64_t(0x03f79d71b4cb0a89);

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
inline int bitScanForward(Bitboard bb) {
   return index64[((bb ^ (bb-1)) * debruijn64) >> 58];
}