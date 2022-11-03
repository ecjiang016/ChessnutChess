#pragma once
#include "bits.h"
#include "masks.h"
#include "magic.h"
#include <vector>

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

std::ostream &operator<<(std::ostream &out, PieceType move);
std::ostream &operator<<(std::ostream &out, Piece move);