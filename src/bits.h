#pragma once
#include <stdint.h>
#include <iostream>
#include <string>

typedef uint64_t Bitboard;

//Byte swap stuff
#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)
/*
#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_64(x) OSSwapInt64(x)
*/
#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_64(x) bswap64(x)
#endif

#else

inline unsigned long long bswap_64(unsigned long long bb) {
    bb = (bb & 0x5555555555555555) << 1 | ((bb >> 1) & 0x5555555555555555);
    bb = (bb & 0x3333333333333333) << 2 | ((bb >> 2) & 0x3333333333333333);
    bb = (bb & 0x0f0f0f0f0f0f0f0f) << 4 | ((bb >> 4) & 0x0f0f0f0f0f0f0f0f);
    bb = (bb & 0x00ff00ff00ff00ff) << 8 | ((bb >> 8) & 0x00ff00ff00ff00ff);

    return (bb << 48) | ((bb & 0xffff0000) << 16) | ((bb >> 16) & 0xffff0000) | (bb >> 48);
}

#endif

//#define check_bit(var, bit_num) (var & (uint64_t(1) << bit_num))

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

//Convert notation through indexing array
const std::string index_to_string[64] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

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