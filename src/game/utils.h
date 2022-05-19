#pragma once
#include <iostream>
#include <string>

//Byte swap stuff
#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_64(x) OSSwapInt64(x)

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

#include "byteswap.h"

#endif

typedef unsigned long long U64;
#define C64(constantU64) constantU64##ULL
#define check_bit(var, bit_num) (var & (C64(1) << bit_num)) //0 if bit at bit_num is 0, > 0 otherwise

enum Piece {
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6
};

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

struct Bitboards {
    U64 whitePawn, blackPawn, whiteKnight, blackKnight, whiteBishop, blackBishop,
        whiteRook, blackRook, whiteQueen, blackQueen, whiteKing, blackKing;

    U64 occupancy, occupancyWhite, occupancyBlack;
    U64 attackWhite, attackBlack;

    U64 en_passant;
    
    Bitboards() {
        whitePawn = C64(0);
        blackPawn = C64(0);
        whiteKnight = C64(0);
        blackKnight = C64(0);
        whiteBishop = C64(0);
        blackBishop = C64(0);
        whiteRook = C64(0);
        blackRook = C64(0);
        whiteQueen = C64(0);
        blackQueen = C64(0);
        whiteKing = C64(0);
        blackKing = C64(0);
        updateOccupancy();
        attackWhite = C64(0);
        attackBlack = C64(0);
        en_passant = C64(0);
    }

    void updateOccupancy() {
        occupancyWhite = whitePawn | whiteKnight | whiteBishop | whiteRook | whiteQueen | whiteKing;
        occupancyBlack = blackPawn | blackKnight | blackBishop | blackRook | blackQueen | blackKing;
        occupancy = occupancyWhite | occupancyBlack;
    }

    U64* getBitboard(bool color, Piece piece) {
        if (color) {
            switch (piece) {
                case 1:
                    return &whitePawn;
                case 2:
                    return &whiteKnight;
                case 3:
                    return &whiteBishop;
                case 4:
                    return &whiteRook;
                case 5:
                    return &whiteQueen;
                case 6:
                    return &whiteKing;
                default:
                    return &whiteKing;
            }
        } else {
            switch (piece) {
                case 1:
                    return &blackPawn;
                case 2:
                    return &blackKnight;
                case 3:
                    return &blackBishop;
                case 4:
                    return &blackRook;
                case 5:
                    return &blackQueen;
                case 6:
                    return &blackKing;
            }
        }

    }

    U64* getBitboard(bool color, int piece) {
        if (color) {
            switch (piece) {
                case 1:
                    return &whitePawn;
                case 2:
                    return &whiteKnight;
                case 3:
                    return &whiteBishop;
                case 4:
                    return &whiteRook;
                case 5:
                    return &whiteQueen;
                case 6:
                    return &whiteKing;
                default:
                    return &whiteKing;
            }
        } else {
            switch (piece) {
                case 1:
                    return &blackPawn;
                case 2:
                    return &blackKnight;
                case 3:
                    return &blackBishop;
                case 4:
                    return &blackRook;
                case 5:
                    return &blackQueen;
                case 6:
                    return &blackKing;
            }
        }

    }

};

const std::string row_str = "  + - + - + - + - + - + - + - + - +";
void print_bitboards(Bitboards& bitboards) {
    int piece_at_pos;
    U64 bitboard_array [12] = {
        bitboards.whitePawn,
        bitboards.blackPawn,
        bitboards.whiteKnight,
        bitboards.blackKnight,
        bitboards.whiteBishop,
        bitboards.blackBishop,
        bitboards.whiteRook,
        bitboards.blackRook,
        bitboards.whiteQueen,
        bitboards.blackQueen,
        bitboards.whiteKing,
        bitboards.blackKing
    };

    std::cout << std::endl;
    for (int row = 7; row >= 0; row--) {
        std::cout << row_str << std::endl;
        std::cout << (row + 1);
        std::cout << " ";

        for (int column = 0; column < 8; column++) {
            std::cout << "| ";

            for (int i = 0; i <= 12; i++) {
                if check_bit(bitboard_array[i], ((row*8) + column)) {
                    piece_at_pos = i;
                    break;
                }
                if (i == 12) {
                    piece_at_pos = 12;
                }
            }

            switch (piece_at_pos) {
                case 0:
                    std::cout << "P";
                    break;
                case 1:
                    std::cout << "p";
                    break;
                case 2:
                    std::cout << "N";
                    break;
                case 3:
                    std::cout << "n";
                    break;
                case 4:
                    std::cout << "B";
                    break;
                case 5:
                    std::cout << "b";
                    break;
                case 6:
                    std::cout << "R";
                    break;
                case 7:
                    std::cout << "r";
                    break;
                case 8:
                    std::cout << "Q";
                    break;
                case 9:
                    std::cout << "q";
                    break;
                case 10:
                    std::cout << "K";
                    break;
                case 11:
                    std::cout << "k"; 
                    break;
                case 12:
                    std::cout << ".";
                    break;
            }

            std::cout << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << row_str << std::endl;
    std::cout << "    a   b   c   d   e   f   g   h" << std::endl << std::endl;
    
};

void print_single_bitboard(U64 bitboard) {
    std::cout << std::endl;
    for (int row = 7; row >= 0; row--) {
        std::cout << row_str << std::endl;
        std::cout << (row + 1);
        std::cout << " ";
    
        for (int column = 0; column < 8; column++) {
            std::cout << "| ";

            if check_bit(bitboard, ((row*8) + column)) {
                std::cout << "1";
            } else {
                std::cout << ".";
            }

            std::cout << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << row_str << std::endl;
    std::cout << "    a   b   c   d   e   f   g   h" << std::endl << std::endl;
    
};

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

const U64 debruijn64 = C64(0x03f79d71b4cb0a89);

/**
 * bitScanForward
 * @author Kim Walisch (2012)
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
inline int bitScanForward(U64 bb) {
   return index64[((bb ^ (bb-1)) * debruijn64) >> 58];
}

inline int file(int square) {
    return square & 7;
}

inline int rank(int square) {
    return square >> 3;
}