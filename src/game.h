#pragma once
#include "moves.h"
#include "bits.h"
#include "piece.h"
#include <vector>

class Chess {
  private:
    Piece mailbox[64];
	Bitboard bitboards[15];
  public:
  	template<Color color> std::vector<Move> getMoves();
    template<Color color> void makeMove(Move move);

};

template<Color color>
void Chess::makeMove(Move move) {
	uint64_t from_to;

	switch (move.flag()) {
		case QUIET:
			from_to = get_single_bitboard(move.from()) | get_single_bitboard(move.to());
			bitboards[mailbox[move.from()]] ^= from_to; // Update piece position on bitboard
			mailbox[move.to()] = mailbox[move.from()]; // Update new mailbox position
			mailbox[move.from()] = NoPiece; // Remove the mailbox piece from it's old position
			break;

	}
}

template<Color color>
std::vector<Move> Chess::getMoves() {
	std::vector<Move> legal_moves;
	legal_moves.reserve(48);
	

}