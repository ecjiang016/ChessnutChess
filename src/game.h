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

	Bitboard bb; //Temp bitboard used for whatever

	const Bitboard friendly = all_bitboards<color>();
	const Bitboard enemy = all_bitboards<~color>();
	const Bitboard all = friendly | enemy;
	
	const int king_square = bitScanForward(get_bitboard(King, color));
	
	//Check for pins and checkers
	Bitboard checkers = 0;
	Bitboard pinned = 0;
	Bitboard danger = 0;

    //Masks out illegal moves due to check
    Bitboard move_mask;

	danger |= pawn_attacks<~color>(get_bitboard(Pawn, ~color));
	danger |= get_attacks<King>(bitScanForward(get_bitboard(King, ~color)));

	//Diagonal attackers
	bb = get_bitboard(Bishop, ~color) | get_bitboard(Queen, ~color);
	while (bb) {
        danger |= get_attacks<Bishop>(bitScanForward(bb), all ^ get_bitboard(King, color)); // xor with King so get xray attacks
        bb &= bb - 1;
    }

	//Straight attackers 
	bb = get_bitboard(Rook, ~color) | get_bitboard(Queen, ~color);
	while (bb) {
        danger |= get_attacks<Rook>(bitScanForward(bb), all ^ get_bitboard(King, color)); // xor with King so get xray attacks
        bb &= bb - 1;
    }

	//Knights
	bb = get_bitboard(Knight, ~color);
	while (bb) {
        danger |= get_attacks<Knight>(bitScanForward(bb), all);
        bb &= bb - 1;
    }

	//Friendly king moves
	bb = get_attacks<King>(king_square, all) & ~(danger | friendly); //Can't go in check or in spaces where friendly pieces are at
	add_moves<QUIET>(king_square, bb & ~enemy, legal_moves);
	add_moves<CAPTURE>(king_square, bb & enemy, legal_moves);

	//Get checkers
	checkers |= get_attacks<Knight>(king_square) & get_bitboard(Knight, ~color); //Look for knights from the king position
	checkers |= pawn_attacks<color>(king_square) & get_bitboard(Pawn, ~color);

	//The potential checkers are the enemy pieces that are in line with the king
	Bitboard pot_checkers = all_direction_masks[king_square] & (get_bitboard(Bishop, ~color) | get_bitboard(Rook, ~color) | get_bitboard(Queen, ~color));
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

        pot_checkers &= pot_checkers - 1; //Remove ls1b
	}

	switch (pop_count(checkers)) {

		//Double check means only king moves are possible
		case 2:
			return legal_moves;

		//Single check
		case 1:
            int checker_idx = bitScanForward(checkers);
            switch(mailbox[checker_idx]) {
                case makePiece(Pawn, ~color):
                    ;
                    //The checking pawn can be taken by en passant
                    //No break to continue to knight case
                case makePiece(Knight, ~color):
                    //If there's a single check from a pawn or a knight, the only moves are captures of that piece
                    //Shouldn't need to include king as it should have been handled earlier
                    //Looks for pieces that can capture by generating attacks from the checker square and looking for intersections with friendly pieces
                    //Essentially generating the capture in "reverse"

                    bb = ((pawn_attacks<~color>(checkers)   & get_bitboard(Pawn,   color)) |
                        (get_attacks<Knight>(checker_idx, all) & get_bitboard(Knight, color)) |
                        (get_attacks<Bishop>(checker_idx, all) & get_bitboard(Bishop, color)) |
                        (get_attacks<Rook>  (checker_idx, all) & get_bitboard(Rook,   color)) |
                        (get_attacks<Queen> (checker_idx, all) & get_bitboard(Queen,  color))) & ~pinned;

                    //Add capture moves to the vector
                    while (bb) {
                        legal_moves.push_back(Move(bitScanForward(bb), checker_idx, QUIET)); // Will always only need to add one move per piece
                        bb &= bb - 1; //Reset ls1b
                    }
                    
                    return legal_moves; //Don't need to generate any other moves

                default:
                    move_mask = connecting_masks[king_square][checker_idx] | checkers;
                    break;
            }

            break;

		//King is not in check
		case 0:
            move_mask = 0; //No moves need to be filtered because of check
            break;

	}

    Bitboard moves; //Temp bitboard to store moves

    //Adding knight moves
    bb = get_bitboard(Knight, color);
    while (bb) {
        moves = get_attacks<Knight>(bitScanForward(bb), all);
        bb &= bb - 1;
    }

}