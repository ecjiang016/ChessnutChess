#pragma once
#include "utils.h"
#include "masks.h"
#include <vector>
#include <string>

struct Move {
    int old_c, new_c, promotion;
    Piece piece;

    Move(int old_coord, int new_coord, Piece piece_type) {
        old_c = old_coord;
        new_c = new_coord;
        promotion = 0;
        piece = piece_type;
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
    }

    std::string UCI() {
        if (!promotion) {
            return index_to_string[old_c] + index_to_string[new_c];
        } else { //Add character at end for promotion
            std::string promotion_str = "Too lazy to actually implement yet";
            return index_to_string[old_c] + index_to_string[new_c] + promotion_str;
        }
    }
};

//The first subtraction is precomputed within the mask
/*
inline U64 sliding_moves(U64 occupancy, U64 mask, U64 piece_square_bitboard) {
    return (((occupancy & mask) - (piece_square_bitboard*2)) ^
        bswap_64(bswap_64(occupancy & mask) - bswap_64(piece_square_bitboard*2))) & mask;
}
*/
inline U64 sliding_moves(U64 occupancy, U64 mask, U64 piece_square_bitboard) {
    return (((occupancy & mask) - (piece_square_bitboard*2)) ^ occupancy) & mask;
}

/**
 * Bitscans the provided bitboard and appends old_c (piece_idx), new_c to the move vector
*/
inline void add_moves(U64 bitboard_moves, int piece_idx, Piece piece, std::vector<Move>& moves) {
    if (bitboard_moves) do {
        int move_idx = bitScanForward(bitboard_moves);
        Move move(piece_idx, move_idx, piece);
        moves.push_back(move);
    } while (bitboard_moves &= bitboard_moves-1);
}

void PawnMoves(Bitboards& bitboards, std::vector<Move>& moves, bool color) {
    int move_idx;
    U64 pawn_bitboard;
    U64 pawn_moves;
    U64 single_push, double_push, right, left, diagonal_right, diagonal_left, en_passant_right, en_passant_left;

    if (color) {
        pawn_bitboard = bitboards.whitePawn;

        single_push = (pawn_bitboard << 8) & ~bitboards.occupancy; //Single push forward
        double_push = ((single_push & 0xFF0000) << 8) & ~bitboards.occupancy; //Double push forward. 0xFF0000 filters out the right row
        right = (pawn_bitboard & 0x7F7F7F7F7F7F7F7F) << 9;
        left = (pawn_bitboard & 0xFEFEFEFEFEFEFEFE) << 7;
        diagonal_right = right & bitboards.occupancyBlack;
        diagonal_left = left & bitboards.occupancyBlack;
        en_passant_right = (bitboards.en_passant << 8) & right & ~bitboards.occupancy;
        en_passant_left = (bitboards.en_passant << 8) & left & ~bitboards.occupancy;
        bitboards.attackWhite |= single_push | double_push | diagonal_right | diagonal_left | en_passant_right | en_passant_left;

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
            Move move(move_idx-9, move_idx, Pawn);
            moves.push_back(move);
        } while (diagonal_right &= diagonal_right-1);

        if (diagonal_left) do {
            move_idx = bitScanForward(diagonal_left);
            Move move(move_idx-7, move_idx, Pawn);
            moves.push_back(move);
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
        pawn_bitboard = bitboards.blackPawn;

        single_push = (pawn_bitboard >> 8) & ~bitboards.occupancy; //Single push forward
        double_push = ((single_push & 0xFF0000000000) >> 8) & ~bitboards.occupancy; //Double push forward. 0xFF0000 filters out the right row
        right = (pawn_bitboard & 0x7F7F7F7F7F7F7F7F) >> 7;
        left = (pawn_bitboard & 0xFEFEFEFEFEFEFEFE) >> 9;
        diagonal_right = right & bitboards.occupancyWhite;
        diagonal_left = left & bitboards.occupancyWhite;
        en_passant_right = (bitboards.en_passant >> 8) & right & ~bitboards.occupancy;
        en_passant_left = (bitboards.en_passant >> 8) & left & ~bitboards.occupancy;
        bitboards.attackBlack |= single_push | double_push | diagonal_right | diagonal_left | en_passant_right | en_passant_left;

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
            Move move(move_idx+7, move_idx, Pawn);
            moves.push_back(move);
        } while (diagonal_right &= diagonal_right-1);

        if (diagonal_left) do {
            move_idx = bitScanForward(diagonal_left);
            Move move(move_idx+9, move_idx, Pawn);
            moves.push_back(move);
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


void KnightMoves(Bitboards& bitboards, std::vector<Move>& moves, bool color) {
    int pos_idx, move_idx;
    U64 knight_moves;
    U64 knight_bitboard = color ? bitboards.whiteKnight : bitboards.blackKnight;
    U64 knight_attacks = C64(0);

    //Move generation using precomputed mask
    //Looping over every knight
    if (knight_bitboard) do {
        pos_idx = bitScanForward(knight_bitboard); //Square index from 0...63
        knight_moves = knight_masks[pos_idx] & ~(color ? bitboards.occupancyWhite : bitboards.occupancyBlack);
        knight_attacks |= knight_moves;

        //Adding all the knight moves
        add_moves(knight_moves, pos_idx, Knight, moves);

    } while (knight_bitboard &= knight_bitboard-1); //Reset the LS1B

    (color ? bitboards.attackWhite : bitboards.attackBlack) |= knight_attacks;
}

void BishopMoves(Bitboards& bitboards, std::vector<Move> moves, bool color) {
    int pos_idx, move_idx;
    U64 bishop_moves, single_bishop_bitboard;
    U64 bishop_bitboard = color ? bitboards.whiteBishop : bitboards.blackBishop;
    U64 bishop_attacks = C64(0);

    //Move generation using precomputed masks and hyperbola quintessence
    if (bishop_bitboard) do {
        single_bishop_bitboard = bishop_bitboard ^ (bishop_bitboard-1);
        pos_idx = index64[(single_bishop_bitboard * debruijn64) >> 58];
        bishop_moves = sliding_moves(bitboards.occupancy, bishop_masks_diag1[pos_idx], single_bishop_bitboard);
        bishop_moves |= sliding_moves(bitboards.occupancy, bishop_masks_diag2[pos_idx], single_bishop_bitboard);

        add_moves(bishop_moves, pos_idx, Bishop, moves);

        bishop_attacks |= bishop_moves;

    } while (bishop_bitboard &= bishop_bitboard-1);

    (color ? bitboards.attackWhite : bitboards.attackBlack) |= bishop_attacks;
}

void RookMoves(Bitboards& bitboards, std::vector<Move>& moves, bool color) {
    int pos_idx, move_idx;
    U64 rook_moves, single_rook_bitboard;
    U64 rook_bitboard = color ? bitboards.whiteRook : bitboards.blackRook;
    U64 rook_attacks = C64(0);
    U64 color_occupancy = color ? bitboards.occupancyWhite : bitboards.occupancyBlack;

    //Move generation using precomputed masks and hyperbola quintessence
    if (rook_bitboard) do {
        single_rook_bitboard = rook_bitboard ^ (rook_bitboard-1);
        pos_idx = index64[(single_rook_bitboard * debruijn64) >> 58];
        //rook_moves = sliding_moves(bitboards.occupancy, rook_masks_horizontal[pos_idx], single_rook_bitboard);
        rook_moves = sliding_moves(bitboards.occupancy, rook_masks_vertical[pos_idx], single_rook_bitboard) ^ color_occupancy;

        add_moves(rook_moves, pos_idx, Rook, moves);

        rook_attacks |= rook_moves;

    } while (rook_bitboard &= rook_bitboard-1);

    (color ? bitboards.attackWhite : bitboards.attackBlack) |= rook_attacks;
}