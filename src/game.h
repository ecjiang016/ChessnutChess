#pragma once
#include "piece.h"
#include "bits.h"
#include <vector>
#include <string>

const std::string starting_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

struct History {
  public:
	//Stores information that the chess game needs to move or undo moves
	uint8_t en_passant_square; //Index of the pawn that can be en passant captured
	Piece capture; //Captured piece for undoing moves
    Bitboard castling;
    /*
    Castling gets initalized to 0x6E1111111111116E
    which includes the inital position of the rooks and kings as 0s and everything else as 1s
    This is used to keep track of if the pieces have been moved or captured to determine if castling is still possible
    */

    inline History() : en_passant_square(0), capture(NoPiece), castling(0x6E1111111111116E) {}

    //When making a new history off of an old one, the only relevant information is the castling square
	//Copy constuctor can only be used for make umake, otherwise copying of the Chess class doesn't work correctly
    //inline History(const History &history) : en_passant_square(0), capture(NoPiece), castling(history.castling) {}
};

class Chess {
  private:
    Piece mailbox[64];
	Bitboard bitboards[15];
    std::vector<History> history;

	template<Color color> inline Bitboard all_bitboards() const;
	constexpr inline Bitboard get_bitboard(PieceType piece, Color color) const {
        return bitboards[makePiece(piece, color)];
    }

  public:
  	template<Color color> std::vector<Move> getMoves() const;
    template<Color color> void makeMove(Move move);
    template<Color color> void unmakeMove(Move move);

	inline std::vector<Piece> getMailbox() const {
        return std::vector<Piece>(mailbox, mailbox + 64);
    }
    
	void setFen(std::string fen);
	std::string getFen() const;
    void print() const;

	inline Chess() {
		setFen(starting_pos);
        history.reserve(200);
        history.push_back(History());
	}

	inline Chess(std::string starting_pos) {
		setFen(starting_pos);
        history.reserve(200);
        history.push_back(History());
	}
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

template<Color color>
void Chess::makeMove(Move move) {
    History current_history_data;
	current_history_data.castling = history.back().castling;

	switch (move.flag()) {
		case QUIET:
			bitboards[mailbox[move.from()]] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to()); // Update piece position on bitboard
			mailbox[move.to()] = mailbox[move.from()]; // Update new mailbox position
			mailbox[move.from()] = NoPiece; // Remove the mailbox piece from it's old position

            //If king or rook makes a quiet move, remove it from the castling bitboard
            current_history_data.castling |= get_single_bitboard(move.from());
			break;

		case CAPTURE:
			bitboards[mailbox[move.from()]] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to());; // Update piece position on bitboard
			bitboards[mailbox[move.to()]] ^= get_single_bitboard(move.to()); //Remove captured piece from its bitboard
            current_history_data.capture = mailbox[move.to()]; //Save the captured piece to the history
			mailbox[move.to()] = mailbox[move.from()]; // Update new mailbox position
			mailbox[move.from()] = NoPiece; // Remove the mailbox piece from it's old position

            //If king or rook makes move, remove it from the castling bitboard 
            //If king or rook gets captured, which means the to position intersects with the castling bitboard, also remove it
            current_history_data.castling |= get_single_bitboard(move.from()) | get_single_bitboard(move.to());

			break;

        //Same as quiet but stores the ending position of the pawn for en passant
        case DOUBLE_PUSH:
            bitboards[makePiece(Pawn, color)] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to()); // Update piece position on bitboard
			mailbox[move.to()] = makePiece(Pawn, color); // Update new mailbox position
			mailbox[move.from()] = NoPiece; // Remove the mailbox piece from it's old position
            current_history_data.en_passant_square = move.to();
			break;

        case EN_PASSANT:
            //Move the pawn
            bitboards[makePiece(Pawn, color)] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to()); // Update piece position on bitboard
            mailbox[move.to()] = makePiece(Pawn, color); // Update new mailbox position
			mailbox[move.from()] = NoPiece; // Remove the mailbox piece from it's old position

            //Get rid of the captured pawn
            if constexpr (color == WHITE) {
                bitboards[makePiece(Pawn, ~color)] ^= get_single_bitboard(move.to() - 8);
                mailbox[move.to() - 8] = NoPiece;
            } else {
                bitboards[makePiece(Pawn, ~color)] ^= get_single_bitboard(move.to() + 8);
                mailbox[move.to() + 8] = NoPiece;
            }

            current_history_data.capture = makePiece(Pawn, ~color); //Save the captured piece to the history

            break;

        case CASTLE_SHORT:
            if constexpr (color == WHITE) {
                bitboards[WhiteKing] ^= Bitboard(0b01010000);
                bitboards[WhiteRook] ^= Bitboard(0b10100000);
                mailbox[4] = NoPiece;
                mailbox[6] = WhiteKing;
                mailbox[7] = NoPiece;
                mailbox[5] = WhiteRook;
                current_history_data.castling |= Bitboard(0b11111111); //Mask out castling for that side
            } else {
                bitboards[BlackKing] ^= Bitboard(0x5000000000000000);
                bitboards[BlackRook] ^= Bitboard(0xA000000000000000);
                mailbox[60] = NoPiece;
                mailbox[62] = WhiteKing;
                mailbox[63] = NoPiece;
                mailbox[61] = WhiteRook;
                current_history_data.castling |= Bitboard(0xFF00000000000000); //Mask out castling for that side
            }
            break;

        case CASTLE_LONG:
            if constexpr (color == WHITE) {
                bitboards[WhiteKing] ^= Bitboard(0b00010100);
                bitboards[WhiteRook] ^= Bitboard(0b00001001);
                mailbox[4] = NoPiece;
                mailbox[2] = WhiteKing;
                mailbox[0] = NoPiece;
                mailbox[3] = WhiteRook;
                current_history_data.castling |= Bitboard(0b11111111);
            } else {
                bitboards[BlackKing] ^= Bitboard(0x1400000000000000);
                bitboards[BlackRook] ^= Bitboard(0x900000000000000);
                mailbox[60] = NoPiece;
                mailbox[58] = WhiteKing;
                mailbox[56] = NoPiece;
                mailbox[59] = WhiteRook;
                current_history_data.castling |= Bitboard(0xFF00000000000000);
            }
            break;

	}

    history.push_back(current_history_data);

}

template<Color color>
void Chess::unmakeMove(Move move) {
    switch (move.flag()) {
        case QUIET:
            bitboards[mailbox[move.to()]] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to()); // Update piece position on bitboard
			mailbox[move.from()] = mailbox[move.to()]; // Update new mailbox position
			mailbox[move.to()] = NoPiece; // Remove the mailbox piece from it's old position
			break;

        case CAPTURE:
            bitboards[mailbox[move.to()]] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to());; // Update piece position on bitboard
			bitboards[history.back().capture] ^= get_single_bitboard(move.to()); //Put back the captured piece
			mailbox[move.from()] = mailbox[move.to()]; // Update new mailbox position
			mailbox[move.to()] = history.back().capture; // Remove the mailbox piece from it's old position
            break;

        case DOUBLE_PUSH:
            bitboards[makePiece(Pawn, color)] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to()); // Update piece position on bitboard
			mailbox[move.from()] = makePiece(Pawn, color); // Update new mailbox position
			mailbox[move.to()] = NoPiece; // Remove the mailbox piece from it's old position
			break;
        
        case EN_PASSANT:
            //Move the pawn
            bitboards[makePiece(Pawn, color)] ^= get_single_bitboard(move.from()) | get_single_bitboard(move.to()); // Update piece position on bitboard
            mailbox[move.from()] = makePiece(Pawn, color); // Update new mailbox position
			mailbox[move.to()] = NoPiece; // Remove the mailbox piece from it's old position

            //Put back the captured pawn
            if constexpr (color == WHITE) {
                bitboards[makePiece(Pawn, ~color)] ^= get_single_bitboard(move.to() - 8);
                mailbox[move.to() - 8] = makePiece(Pawn, ~color);
            } else {
                bitboards[makePiece(Pawn, ~color)] ^= get_single_bitboard(move.to() + 8);
                mailbox[move.to() + 8] = makePiece(Pawn, ~color);
            }

            break;

        case CASTLE_SHORT:
            if constexpr (color == WHITE) {
                bitboards[WhiteKing] ^= Bitboard(0b01010000);
                bitboards[WhiteRook] ^= Bitboard(0b10100000);
                mailbox[4] = WhiteKing;
                mailbox[6] = NoPiece;
                mailbox[7] = WhiteRook;
                mailbox[5] = NoPiece;
            } else {
                bitboards[BlackKing] ^= Bitboard(0x5000000000000000);
                bitboards[BlackRook] ^= Bitboard(0xA000000000000000);
                mailbox[60] = WhiteKing;
                mailbox[62] = NoPiece;
                mailbox[63] = WhiteRook;
                mailbox[61] = NoPiece;
            }
            break;

        case CASTLE_LONG:
            if constexpr (color == WHITE) {
                bitboards[WhiteKing] ^= Bitboard(0b00010100);
                bitboards[WhiteRook] ^= Bitboard(0b00001001);
                mailbox[4] = WhiteKing;
                mailbox[2] = NoPiece;
                mailbox[0] = WhiteRook;
                mailbox[3] = NoPiece;

            } else {
                bitboards[BlackKing] ^= Bitboard(0x1400000000000000);
                bitboards[BlackRook] ^= Bitboard(0x900000000000000);
                mailbox[60] = WhiteKing;
                mailbox[58] = NoPiece;
                mailbox[56] = WhiteRook;
                mailbox[59] = NoPiece;
            }
            break;
    }

    history.pop_back();

}

template<Color color>
std::vector<Move> Chess::getMoves() const {
	std::vector<Move> legal_moves;
	legal_moves.reserve(48);

	Bitboard bb; //Temp bitboard used for whatever
	Bitboard moves; //Temp bitboard to store moves
    int pos; //Temp int for storing positions

	const Bitboard friendly = all_bitboards<color>();
	const Bitboard enemy = all_bitboards<~color>();
	const Bitboard all = friendly | enemy;
	
	const int king_square = bitScanForward(get_bitboard(King, color));
	
	//Check for pins and checkers
	Bitboard checkers = 0;
	Bitboard pinned = 0;
	Bitboard danger = 0;

    //For masking moves to either being a quiet or a capture move
    Bitboard quiet_mask;
    Bitboard capture_mask;

	danger |= pawn_attacks<~color>(get_bitboard(Pawn, ~color));
	danger |= get_attacks<King>(bitScanForward(get_bitboard(King, ~color)), all);

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
	checkers |= get_attacks<Knight>(king_square, all) & get_bitboard(Knight, ~color); //Look for knights from the king position
	checkers |= pawn_attacks<color>(get_bitboard(King, color)) & get_bitboard(Pawn, ~color);

	//The potential checkers are the enemy pieces that are in line with the king
	bb = ((rook_masks_horizontal[king_square] | rook_masks_vertical[king_square]) & (get_bitboard(Rook, ~color) | get_bitboard(Queen, ~color))) |
         ((bishop_masks_diag1[king_square] | bishop_masks_diag2[king_square]) & (get_bitboard(Bishop, ~color) | get_bitboard(Queen, ~color)));
	while (bb) {
		moves = connecting_masks[king_square][bitScanForward(bb)] & all; //moves is actually the pieces in between the king and the checker
		switch (pop_count(moves)) {
			case 0:
				checkers |= bb & -bb;
				break;
			case 1:
				pinned |= moves & friendly;
				break;
		}
        bb &= bb - 1; //Remove ls1b
	}

	switch (pop_count(checkers)) {

		//Double check means only king moves are possible
		case 2:
			return legal_moves;

		//Single check
		case 1:
            pos = bitScanForward(checkers); //Index of the checker
            switch(mailbox[pos]) {
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
                        (get_attacks<Knight>(pos, all) & get_bitboard(Knight, color)) |
                        (get_attacks<Bishop>(pos, all) & (get_bitboard(Bishop, color) | get_bitboard(Queen, color))) |
                        (get_attacks<Rook>  (pos, all) & (get_bitboard(Rook,   color) | get_bitboard(Queen, color)))) & ~pinned;

                    //Add capture moves to the vector
                    while (bb) {
                        legal_moves.push_back(Move(bitScanForward(bb), pos, CAPTURE)); // Will always only need to add one move per piece
                        bb &= bb - 1; //Reset ls1b
                    }
                    
                    return legal_moves; //Don't need to generate any other moves

                default:
                    quiet_mask = connecting_masks[king_square][pos]; //Only quiet moves are moving in between the checker and the king
                    capture_mask = checkers; //Only capture move is to capture the checker
                    break;
            }

            break;

		//King is not in check
		case 0:
            quiet_mask = ~all; //Quiet moves are on empty spaces
            capture_mask = enemy;

            //Castling is only possible when there's no check
            
            //castling_pieces is a mask that includes the white king and the castling rook
            //king_castle_spaces is a mask that includes the spaces in between the castling rook and the king
            
            //history.back().castling & castling_pieces will evaluate to 0 if the king and rook haven't moved
            //(danger | all) & king_castle_spaces will evaulate to 0 if the king won't get into check or get blocked on the way it's castle position
            //Castling is possible if ~piece_moved & ~danger which is logically equivalent to ~(piece_moved | danger) or !(piece_moved | danger) to cast to bool
            
            if (!((history.back().castling & castling_pieces<color, CASTLE_SHORT>()) |
                ((danger | all) & king_castle_spaces<color, CASTLE_SHORT>()))) {

                if constexpr (color == WHITE) {
                    legal_moves.push_back(Move(4, 6, CASTLE_SHORT));
                } else {
                    legal_moves.push_back(Move(60, 62, CASTLE_SHORT));
                }

            }

            //For long castles, the space where the knight is doesn't have to be clear of checks for castling to be possible
            //So it gets masked out by intersecting it with ~long_castle_knight<color>()
            if (!((history.back().castling & castling_pieces<color, CASTLE_LONG>()) |
                (((danger & ~long_castle_knight<color>()) | all) & king_castle_spaces<color, CASTLE_LONG>()))) {

                if constexpr (color == WHITE) {
                    legal_moves.push_back(Move(4, 2, CASTLE_LONG));
                } else {
                    legal_moves.push_back(Move(60, 58, CASTLE_LONG));
                }

            }


            //Pins only have to be taken care of when there isn't a check
            //Pinned pieces can only move in the squares between the checker it's blocking and the king
            
            //Pinned pawn moves
            bb = get_bitboard(Pawn, color) & pinned;
            while (bb) {
                pos = bitScanForward(bb);

				//Pawn pushes
				if constexpr (color == WHITE) {
					moves = ((bb & -bb) << 8) & quiet_mask & ray_masks[king_square][pos]; //Single pawn push
					//If single push is in the direction of the pin, the second push will be as well
					if (((moves & Bitboard(0xFF0000)) << 8) & quiet_mask) { //Double pawn push generated off of single pawn push
						legal_moves.push_back(Move(pos, pos + 16, DOUBLE_PUSH));
					}
					if (moves & quiet_mask) { //Add single pawn push
						legal_moves.push_back(Move(pos, pos + 8, QUIET));
					}

				} else {
					moves = ((bb & -bb) >> 8) & quiet_mask & ray_masks[king_square][pos]; //Single pawn push
					//If single push is in the direction of the pin, the second push will be as well
					if (((moves & Bitboard(0xFF0000000000)) >> 8) & quiet_mask) { //Double pawn push generated off of single pawn push
						legal_moves.push_back(Move(pos, pos - 16, DOUBLE_PUSH));
					}
					if (moves & quiet_mask) { //Add single pawn push
						legal_moves.push_back(Move(pos, pos - 8, QUIET));
					}
				}
                
                //Pawn attacks
                moves = pawn_attacks<color>(bb & -bb) & ray_masks[king_square][pos];
                add_moves<CAPTURE>(bitScanForward(bb), moves & capture_mask, legal_moves);

                //Handle en passants
                if constexpr (color == WHITE) {
                    if ((moves >> 8) & get_single_bitboard(history.back().en_passant_square)) {
                        legal_moves.push_back(Move(pos, history.back().en_passant_square + 8, EN_PASSANT)); //Only one en passant can be possible per turn
                    }
                } else {
                    if ((moves << 8) & get_single_bitboard(history.back().en_passant_square)) {
                        legal_moves.push_back(Move(pos, history.back().en_passant_square - 8, EN_PASSANT)); //Only one en passant can be possible per turn
                    }
                }
                bb &= bb - 1;
            }

			//Pinned knights can't move, don't need to generate any moves for them

			//Adding bishop + diagonal queen moves
			bb = (get_bitboard(Bishop, color) | get_bitboard(Queen, color)) & pinned;
			while (bb) {
				pos = bitScanForward(bb);
				moves = get_attacks<Bishop>(pos, all) & ray_masks[king_square][pos];
				add_moves<QUIET>(pos, moves & quiet_mask, legal_moves);
				//Add captures
				//There's a max of one capture possible for pinned pieces
				if (moves & capture_mask) {
					legal_moves.push_back(Move(pos, bitScanForward(moves & capture_mask), CAPTURE));
				}
				bb &= bb - 1;
			}

			//Adding bishop + diagonal queen moves
			bb = (get_bitboard(Rook, color) | get_bitboard(Queen, color)) & pinned;
			while (bb) {
				pos = bitScanForward(bb);
				moves = get_attacks<Rook>(pos, all) & ray_masks[king_square][pos];
				add_moves<QUIET>(pos, moves & quiet_mask, legal_moves);
				//Add captures
				//There's a max of one capture possible for pinned pieces
				if (moves & capture_mask) {
					legal_moves.push_back(Move(pos, bitScanForward(moves & capture_mask), CAPTURE));
				}
				bb &= bb - 1;
			}

            break;

	}

	//Generate pawn moves
	if constexpr (color == WHITE) {
		bb = ((bitboards[WhitePawn] & ~pinned) << 8) & ~all; //Single pawn push
		moves = ((bb & Bitboard(0xFF0000)) << 8) & quiet_mask; //Double pawn push generated off of single pawn push
        bb &= quiet_mask;
		
		while (bb) {
			pos = bitScanForward(bb);
			legal_moves.push_back(Move(pos - 8, pos));
			bb &= bb - 1;
		}

		while (moves) {
			pos = bitScanForward(moves);
			legal_moves.push_back(Move(pos - 16, pos, DOUBLE_PUSH));
			moves &= moves - 1;
		}

	} else {
		bb = ((bitboards[BlackPawn] & ~pinned) >> 8) & ~all; //Single pawn push
		moves = ((bb & Bitboard(0xFF0000000000)) >> 8) & quiet_mask; //Double pawn push generated off of single pawn push
        bb &= quiet_mask;

		while (bb) {
			pos = bitScanForward(bb);
			legal_moves.push_back(Move(pos + 8, pos));
			bb &= bb - 1;
		}

		while (moves) {
			pos = bitScanForward(moves);
			legal_moves.push_back(Move(pos + 16, pos, DOUBLE_PUSH));
			moves &= moves - 1;
		}

	}

	//Add pawn attacks
	bb = get_bitboard(Pawn, color) & ~pinned;
    while (bb) {
        moves = pawn_attacks<color>(bb & -bb);
        add_moves<CAPTURE>(bitScanForward(bb), moves & capture_mask, legal_moves);
        //Handle en passants
        if constexpr (color == WHITE) {
            if ((moves >> 8) & get_single_bitboard(history.back().en_passant_square)) {
                legal_moves.push_back(Move(bitScanForward(bb), history.back().en_passant_square + 8, EN_PASSANT)); //Only one en passant can be possible per piece
            }
        } else {
            if ((moves << 8) & get_single_bitboard(history.back().en_passant_square)) {
                legal_moves.push_back(Move(bitScanForward(bb), history.back().en_passant_square - 8, EN_PASSANT)); //Only one en passant can be possible per piece
            }
        }
        bb &= bb - 1;
    }

    //Adding knight moves
    bb = get_bitboard(Knight, color) & ~pinned;
    while (bb) {
        pos = bitScanForward(bb);
        moves = get_attacks<Knight>(pos, all);
        add_moves<QUIET>(pos, moves & quiet_mask, legal_moves);
        add_moves<CAPTURE>(pos, moves & capture_mask, legal_moves);
        bb &= bb - 1;
    }

    //Adding bishop + diagonal queen moves
    bb = (get_bitboard(Bishop, color) | get_bitboard(Queen, color)) & ~pinned;
    while (bb) {
        pos = bitScanForward(bb);
        moves = get_attacks<Bishop>(pos, all);
        add_moves<QUIET>(pos, moves & quiet_mask, legal_moves);
        add_moves<CAPTURE>(pos, moves & capture_mask, legal_moves);
        bb &= bb - 1;
    }
    
    //Adding rook + not diagonal queen moves
    bb = (get_bitboard(Rook, color) | get_bitboard(Queen, color)) & ~pinned;
    while (bb) {
        pos = bitScanForward(bb);
        moves = get_attacks<Rook>(pos, all);
        add_moves<QUIET>(pos, moves & quiet_mask, legal_moves);
        add_moves<CAPTURE>(pos, moves & capture_mask, legal_moves);
        bb &= bb - 1;
    }

    return legal_moves;

}