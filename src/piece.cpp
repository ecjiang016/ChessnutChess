#include "piece.h"

std::ostream &operator<<(std::ostream &out, PieceType piece) {
    switch (piece) {
        case NoPieceType:
            return out << "No Piece";
        case Pawn:
            return out << "Pawn";
        case Knight:
            return out << "Knight";
        case Bishop:
            return out << "Bishop";
        case Rook:
            return out << "Rook";
        case Queen:
            return out << "Queen";
        case King:
            return out << "King";
        default:
            return out;
    }
}

std::ostream &operator<<(std::ostream &out, Piece piece) {
    switch (getPieceColor(piece)) {
        case WHITE:
            out << "White ";
        case BLACK:
            out << "Black ";
    }
    return out << getPieceType(piece);
}