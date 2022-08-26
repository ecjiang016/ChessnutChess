#include "game.h"

constexpr inline Bitboard Chess::get_bitboard(PieceType piece, Color color) const {
	return bitboards[makePiece(piece, color)];
}

std::vector<Piece> Chess::getMailbox() const {
    return std::vector<Piece>(mailbox, mailbox + 64);
}