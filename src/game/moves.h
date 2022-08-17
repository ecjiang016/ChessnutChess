#pragma once
#include "utils.h"
#include "masks.h"
#include "tables.h"
#include <vector>
#include <string>

class Chess;

struct Move {
    int old_c, new_c, promotion;
    Piece piece;
    Piece capture;

    Move() {
        old_c = -1;
        new_c = -1;
        promotion = 0;
        piece = None;
        capture = None;
    }

    Move(int old_coord, int new_coord, Piece piece_type) {
        old_c = old_coord;
        new_c = new_coord;
        promotion = 0;
        piece = piece_type;
        capture = None;
    }

    Move(int old_coord, int new_coord, Piece piece_type, Piece capture_type) {
        old_c = old_coord;
        new_c = new_coord;
        promotion = 0;
        piece = piece_type;
        capture = capture_type;
    }

    Move(std::string long_algebraic_move, Piece piece_type) {
        for (int i = 0; i < 64; i++) {
            if (long_algebraic_move.substr(0, 2) == index_to_string[i]) {
                old_c = i;
            }
            if (long_algebraic_move.substr(2, 2) == index_to_string[i]) {
                new_c = i;
            }
        }
        promotion = 0;
        piece = piece_type;
        capture = None;
    }

    std::string UCI() {
        if (!promotion) {
            return index_to_string[old_c] + index_to_string[new_c];
        } else { //Add character at end for promotion
            std::string promotion_str = "Too lazy to actually implement yet";
            return index_to_string[old_c] + index_to_string[new_c] + promotion_str;
        }
    }

    std::string AN() {
        std::string piece_char = "";
        switch (piece) {
            case Pawn:
                piece_char = "";
                break;
            case Knight:
                piece_char = "N";
                break;
            case Bishop:
                piece_char = "B";
                break;
            case Rook:
                piece_char = "R";
                break;
            case Queen:
                piece_char = "Q";
                break;
            case King:
                piece_char = "K";
                break;
        }
        return piece_char + index_to_string[new_c];
    }
};

//The first subtraction is precomputed within the mask
inline U64 sliding_moves(U64 occupancy, U64 mask, U64 piece_square_bitboard) {
    return (((occupancy & mask) - piece_square_bitboard) ^
        bswap_64(bswap_64(occupancy & mask) - bswap_64(piece_square_bitboard))) & mask;
}

/**
 * Bitscans the provided bitboard and appends old_c (piece_idx), new_c to the move vector
*/
inline void add_moves(U64 bitboard_moves, int piece_idx, Piece piece, std::vector<Move>& moves, bool color, Bitboards* bitboards) {
    if (bitboard_moves) do {
        int move_idx = bitScanForward(bitboard_moves);
        U64 check_bb = bitboard_moves & -bitboard_moves;

        if (!(check_bb & bitboards->occupancy)) { //If move is not a capture
            Move move(piece_idx, move_idx, piece);
            moves.push_back(move);
        } else {
            for (int p = 1; p <= 7; p++) {
                if (check_bb & *(bitboards->getBitboard(!color, p))) {
                    Piece capture = static_cast<Piece>(p);
                    Move move(piece_idx, move_idx, piece, capture);
                    moves.push_back(move);
                    break;
                }
            }
        }
        
    } while (bitboard_moves &= bitboard_moves-1);
}

void PawnMoves(Bitboards& bitboards, std::vector<Move>& moves, bool color, attackTable& attack_table, U64 move_mask, U64 pinned_pieces) {
    int move_idx;
    U64 single_push, double_push, right, left, diagonal_right, diagonal_left, en_passant_right, en_passant_left;

    if (color) {
        U64 pawn_bitboard = bitboards.whitePawn & ~pinned_pieces;
        single_push = (pawn_bitboard << 8) & ~bitboards.occupancy; //Single push forward
        double_push = ((single_push & 0xFF0000) << 8) & ~bitboards.occupancy & move_mask; //Double push forward. 0xFF0000 filters out the right row
        single_push &= move_mask;
        right = (pawn_bitboard & 0x7F7F7F7F7F7F7F7F) << 9;
        left = (pawn_bitboard & 0xFEFEFEFEFEFEFEFE) << 7;
        diagonal_right = right & bitboards.occupancyBlack & move_mask;
        diagonal_left = left & bitboards.occupancyBlack & move_mask;
        en_passant_right = (bitboards.en_passant << 8) & right & ~bitboards.occupancy & move_mask;
        en_passant_left = (bitboards.en_passant << 8) & left & ~bitboards.occupancy & move_mask;
        attack_table.whitePawn = right | left;

        U64 pinned_pawns = bitboards.whitePawn & pinned_pieces;
        if (pinned_pawns) do { //Pinned pawns can only capture
            int king_idx = bitScanForward(bitboards.whiteKing);
            int piece_idx = bitScanForward(pinned_pawns);
            U64 single_pawn = pinned_pawns & -pinned_pawns;
            diagonal_right |= ((single_pawn & 0x7F7F7F7F7F7F7F7F) << 9) &
                bitboards.occupancyBlack & move_mask & connecting_full_masks[king_idx][piece_idx];
            diagonal_left |= ((single_pawn & -pinned_pawns & 0xFEFEFEFEFEFEFEFE) << 7) &
                bitboards.occupancyBlack & move_mask & connecting_full_masks[king_idx][piece_idx];
            single_push |= (pinned_pawns << 8) & ~bitboards.occupancy & connecting_full_masks[king_idx][piece_idx];
            double_push |= ((single_push & 0xFF0000) << 8) & ~bitboards.occupancy & move_mask;
            single_push &= move_mask;
        } while (pinned_pawns &= pinned_pawns - 1);

        if (single_push) do {
            move_idx = bitScanForward(single_push);
            Move move(move_idx-8, move_idx, Pawn);
            moves.push_back(move);
        } while (single_push &= single_push-1);

        if (double_push) do {
            move_idx = bitScanForward(double_push);
            Move move(move_idx-16, move_idx, Pawn);
            moves.push_back(move);
        } while (double_push &= double_push-1);

        if (diagonal_right) do {
            move_idx = bitScanForward(diagonal_right);
            for (int p = 1; p <= 6; p++) {
                if ((diagonal_right & -diagonal_right) & *(bitboards.getBitboard(!color, p))) {
                    Piece capture = static_cast<Piece>(p);
                    Move move(move_idx-9, move_idx, Pawn, capture);
                    moves.push_back(move);
                    break;
                }
            }
        } while (diagonal_right &= diagonal_right-1);

        if (diagonal_left) do {
            move_idx = bitScanForward(diagonal_left);
            for (int p = 1; p <= 6; p++) {
                if ((diagonal_left & -diagonal_left) & *(bitboards.getBitboard(!color, p))) {
                    Piece capture = static_cast<Piece>(p);
                    Move move(move_idx-7, move_idx, Pawn, capture);
                    moves.push_back(move);
                    break;
                }
            }
        } while (diagonal_left &= diagonal_left-1);

        if (en_passant_right) do {
            move_idx = bitScanForward(en_passant_right);
            Move move(move_idx-9, move_idx, Pawn);
            moves.push_back(move);
        } while (en_passant_right &= en_passant_right-1);

        if (en_passant_left) do {
            move_idx = bitScanForward(en_passant_left);
            Move move(move_idx-7, move_idx, Pawn);
            moves.push_back(move);
        } while (en_passant_left &= en_passant_left-1);

    } else {
        //Same thing but modified for black
        U64 pawn_bitboard = bitboards.blackPawn & ~pinned_pieces;
        single_push = (pawn_bitboard >> 8) & ~bitboards.occupancy; //Single push forward
        double_push = ((single_push & 0xFF0000000000) >> 8) & ~bitboards.occupancy & move_mask; //Double push forward. 0xFF0000 filters out the right row
        single_push &= move_mask;
        right = (pawn_bitboard & 0x7F7F7F7F7F7F7F7F) >> 7;
        left = (pawn_bitboard & 0xFEFEFEFEFEFEFEFE) >> 9;
        diagonal_right = right & bitboards.occupancyWhite & move_mask;
        diagonal_left = left & bitboards.occupancyWhite & move_mask;
        en_passant_right = (bitboards.en_passant >> 8) & right & ~bitboards.occupancy & move_mask;
        en_passant_left = (bitboards.en_passant >> 8) & left & ~bitboards.occupancy & move_mask;
        attack_table.blackPawn = right | left;

        U64 pinned_pawns = bitboards.blackPawn & pinned_pieces;
        if (pinned_pawns) do { //Pinned pawns can only capture
            int king_idx = bitScanForward(bitboards.blackKing);
            int piece_idx = bitScanForward(pinned_pawns);
            U64 single_pawn = pinned_pawns & -pinned_pawns;
            diagonal_right |= ((single_pawn & 0x7F7F7F7F7F7F7F7F) >> 7) &
                bitboards.occupancyWhite & move_mask & connecting_full_masks[king_idx][piece_idx];
            diagonal_left |= ((single_pawn & -pinned_pawns & 0xFEFEFEFEFEFEFEFE) >> 9) &
                bitboards.occupancyWhite & move_mask & connecting_full_masks[king_idx][piece_idx];
            single_push |= (pinned_pawns >> 8) & ~bitboards.occupancy & connecting_full_masks[king_idx][piece_idx];
            double_push |= ((single_push & 0xFF0000000000) >> 8) & ~bitboards.occupancy & move_mask;
            single_push &= move_mask;
        } while (pinned_pawns &= pinned_pawns - 1);

        if (single_push) do {
            move_idx = bitScanForward(single_push);
            Move move(move_idx+8, move_idx, Pawn);
            moves.push_back(move);
        } while (single_push &= single_push-1);

        if (double_push) do {
            move_idx = bitScanForward(double_push);
            Move move(move_idx+16, move_idx, Pawn);
            moves.push_back(move);
        } while (double_push &= double_push-1);

        if (diagonal_right) do {
            move_idx = bitScanForward(diagonal_right);
            for (int p = 1; p <= 6; p++) {
                if ((diagonal_right & -diagonal_right) & *(bitboards.getBitboard(!color, p))) {
                    Piece capture = static_cast<Piece>(p);
                    Move move(move_idx+7, move_idx, Pawn, capture);
                    moves.push_back(move);
                    break;
                }
            }
        } while (diagonal_right &= diagonal_right-1);

        if (diagonal_left) do {
            move_idx = bitScanForward(diagonal_left);
            for (int p = 1; p <=6; p++) {
                if ((diagonal_left & -diagonal_left) & *(bitboards.getBitboard(!color, p))) {
                    Piece capture = static_cast<Piece>(p);
                    Move move(move_idx+9, move_idx, Pawn, capture);
                    moves.push_back(move);
                    break;
                }
            }
        } while (diagonal_left &= diagonal_left-1);

        if (en_passant_right) do {
            move_idx = bitScanForward(en_passant_right);
            Move move(move_idx+7, move_idx, Pawn);
            moves.push_back(move);
        } while (en_passant_right &= en_passant_right-1);

        if (en_passant_left) do {
            move_idx = bitScanForward(en_passant_left);
            Move move(move_idx+9, move_idx, Pawn);
            moves.push_back(move);
        } while (en_passant_left &= en_passant_left-1);

    }
}

void KnightMoves(U64 knight_bitboard, U64 color_occupancy, std::vector<Move>& moves, bool color, attackTable& attack_table, Bitboards* bb_ptr, U64 move_mask) {
    int pos_idx;
    U64 knight_moves;

    //Move generation using precomputed mask
    pos_idx = bitScanForward(knight_bitboard); //Square index from 0...63
    knight_moves = knight_masks[pos_idx] & ~color_occupancy & move_mask;
    (color ? attack_table.whiteKnight : attack_table.blackKnight) |= knight_masks[pos_idx];

    //Adding all the knight moves
    add_moves(knight_moves, pos_idx, Knight, moves, color, bb_ptr);

}

void BishopMoves(U64 bishop_bitboard, U64 occupancy, U64 color_occupancy, std::vector<Move>& moves, bool color, attackTable& attack_table, Bitboards* bb_ptr, U64 move_mask, U64 pinned_pieces) {
    int pos_idx;
    U64 bishop_moves;

    //Move generation using precomputed masks and hyperbola quintessence
    pos_idx = bitScanForward(bishop_bitboard);
    bishop_moves = sliding_moves(occupancy, bishop_masks_diag1[pos_idx], (bishop_bitboard & -bishop_bitboard)) |
        sliding_moves(occupancy, bishop_masks_diag2[pos_idx], (bishop_bitboard & -bishop_bitboard));
    (color ? attack_table.whiteBishop : attack_table.blackBishop) |= bishop_moves;

    if (pinned_pieces & (bishop_bitboard & -bishop_bitboard)) {
        int king_idx = bitScanForward(color ? bb_ptr->whiteKing : bb_ptr->blackKing);
        bishop_moves &= connecting_full_masks[king_idx][pos_idx];
    }

    add_moves(bishop_moves & ~color_occupancy & move_mask, pos_idx, Bishop, moves, color, bb_ptr);
}

void RookMoves(U64 rook_bitboard, U64 occupancy, U64 color_occupancy, std::vector<Move>& moves, bool color, attackTable& attack_table, Bitboards* bb_ptr, U64 move_mask, U64 pinned_pieces) {
    int pos_idx;
    U64 rook_moves;

    //Move generation using precomputed masks and hyperbola quintessence
    pos_idx = bitScanForward(rook_bitboard);
    rook_moves = sliding_moves(occupancy, rook_masks_vertical[pos_idx], (rook_bitboard & -rook_bitboard)) | 
        sliding_moves(occupancy, rook_masks_horizontal[pos_idx], (rook_bitboard & -rook_bitboard));

    if (pinned_pieces & (rook_bitboard & -rook_bitboard)) {
        int king_idx = bitScanForward(color ? bb_ptr->whiteKing : bb_ptr->blackKing);
        rook_moves &= connecting_full_masks[king_idx][pos_idx];
    }

    add_moves(rook_moves & ~color_occupancy & move_mask, pos_idx, Rook, moves, color, bb_ptr);

    (color ? attack_table.whiteRook : attack_table.blackRook) |= rook_moves;

}

void QueenMoves(U64 queen_bitboard, U64 occupancy, U64 color_occupancy, std::vector<Move>& moves, bool color, attackTable& attack_table, Bitboards* bb_ptr, U64 move_mask, U64 pinned_pieces) {
    int pos_idx;
    U64 queen_moves;

    //Move generation using precomputed masks and hyperbola quintessence
    pos_idx = bitScanForward(queen_bitboard);
    queen_moves = sliding_moves(occupancy, rook_masks_vertical[pos_idx], (queen_bitboard & -queen_bitboard)) |
        sliding_moves(occupancy, rook_masks_horizontal[pos_idx], (queen_bitboard & -queen_bitboard)) |
        sliding_moves(occupancy, bishop_masks_diag1[pos_idx], (queen_bitboard & -queen_bitboard)) |
        sliding_moves(occupancy, bishop_masks_diag2[pos_idx], (queen_bitboard & -queen_bitboard));

    if (pinned_pieces & (queen_bitboard & -queen_bitboard)) {
        int king_idx = bitScanForward(color ? bb_ptr->whiteKing : bb_ptr->blackKing);
        queen_moves &= connecting_full_masks[king_idx][pos_idx];
    }

    add_moves(queen_moves & ~color_occupancy & move_mask, pos_idx, Queen, moves, color, bb_ptr);

    (color ? attack_table.whiteQueen : attack_table.blackQueen) |= queen_moves;

}

void KingMoves(U64 king_bitboard, U64 color_occupancy, std::vector<Move>& moves, bool color, attackTable& attack_table, Bitboards* bb_ptr) {
    int pos_idx;
    U64 king_moves;

    //Move generation using precomputed mask
    pos_idx = bitScanForward(king_bitboard); //Square index from 0...63
    king_moves = king_masks[pos_idx] & ~color_occupancy & ~(color ? attack_table.getBlackAttacks() : attack_table.getWhiteAttacks());
    (color ? attack_table.whiteKing : attack_table.blackKing) = king_masks[pos_idx];

    add_moves(king_moves, pos_idx, King, moves, color, bb_ptr);
}