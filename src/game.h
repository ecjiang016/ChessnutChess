#pragma once
#include "moves.h"
#include "bits.h"
#include <vector>

class Chess {
  private:
    Piece mailbox[64];
	Bitboard bitboards[15];

	template<Color color> inline Bitboard all_bitboards() const;
	template<PieceType piece, Color color> inline Bitboard get_bitboard() const;

  public:
  	template<Color color> std::vector<Move> getMoves() const;
    template<Color color> void makeMove(Move move);

};

//Returns all the bitboards of a certain color
template<Color color>
inline Bitboard Chess::all_bitboards() const {
	if constexpr (color == WHITE) {
		return bitboards[WhitePawn] | bitboards[WhiteKnight] | bitboards[WhiteBishop] | 
			   bitboards[WhiteRook] | bitboards[WhiteQueen]  | bitboards[WhiteKing];
	} else {
		return bitboards[BlackPawn] | bitboards[BlackKnight] | bitboards[BlackBishop] |
			   bitboards[BlackRook] | bitboards[BlackQueen]  | bitboards[BlackKing];
	} 
}

template<PieceType piece, Color color> inline Bitboard Chess::get_bitboard() const {
	return bitboards[makePiece(piece, color)];
}

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
std::vector<Move> Chess::getMoves() const {
	std::vector<Move> legal_moves;
	legal_moves.reserve(48);

	Bitboard bb; // Temp bitboard used for whatever

	const Bitboard friendly = all_bitboards<color>();
	const Bitboard enemy = all_bitboards<~color>();
	const Bitboard all = friendly | enemy;
	
	const uint8_t king_square = bitScanForward(get_bitboard(King, color));
	
	//Check for pins and checkers
	Bitboard checkers = 0;
	Bitboard pinned = 0;
	Bitboard danger = 0;

	danger |= pawn_attacks<~color>(get_bitboard(Pawn, ~color));
	danger |= get_attacks<King>(get_bitboard(King, ~color));

	//Diagonal attackers
	bb = get_bitboard(Bishop, ~color) | get_bitboard(Queen, ~color);
	while (bb &= bb-1) danger |= get_attacks<Bishop>(bb, all ^ get_bitboard(King, color)); // xor with King so get xray attacks
	//Straight attackers 
	bb = get_bitboard(Rook, ~color) | get_bitboard(Queen, ~color);
	while (bb &= bb-1) danger |= get_attacks<Rook>(bb, all ^ get_bitboard(King, color)); // xor with King so get xray attacks
	//Knights
	bb = get_bitboard(Knight, ~color);
	while (bb &= bb-1) danger |= get_attacks<Knight>(knight_attackers, all);

	//Friendly king moves
	bb = get_attacks<King>(get_bitboard(King, color), all) & ~(danger | friendly); //Can't go in check or in spaces where friendly pieces are at
	add_moves<QUIET>(king_square, bb & ~enemy, legal_moves);
	add_moves<CAPTURE>(king_square, bb & enemy, legal_moves);

	//Get checkers
	checkers |= get_attacks<Knight>(get_bitboard(King, color)) & get_bitboard(King, color); //Look for knights from the king position

	//The potential checkers are the enemy pieces that are in line with the kiing
	Bitboard pot_checkers = get_attacks<Queen>(get_bitboard(King, color)) & (get_bitboard(Bishop, ~color) | get_bitboard(Rook, ~color) | get_bitboard(Queen, ~color));
	while (pot_checkers) {
		uint8_t pieces_between = connecting_masks[king_square][bitScanForward(pot_checkers)] & all;
		switch (pop_count(pieces_between)) {
			case 0:
				checkers |= pot_checkers & -pot_checkers;
				break;
			case 1:
				pinned |= pieces_between & friendly;
				break;
		}
	}

	switch (pop_count(checkers)) {

		//Double check means only king moves are possible
		case 2:
			return legal_moves;

		//Single check
		case 1:


		//King is not in check
		case 0:


	}

}