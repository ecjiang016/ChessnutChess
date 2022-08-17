#pragma once
#include "utils.h"
#include "masks.h"

inline U64 sliding_moves(U64 occupancy, U64 mask, U64 piece_square_bitboard);

struct attackTable {
    U64 whitePawn, blackPawn, whiteKnight, blackKnight, whiteBishop, blackBishop,
        whiteRook, blackRook, whiteQueen, blackQueen, whiteKing, blackKing;

    attackTable() {
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
    }

    U64 getWhiteAttacks() {
        return whitePawn | whiteKnight | whiteBishop | whiteRook | whiteQueen | whiteKing;
    }

    U64 getBlackAttacks() {
        return blackPawn | blackKnight | blackBishop | blackRook | blackQueen | blackKing;
    }

    void clearTables(bool color) {
        if (color) {
            whitePawn = C64(0);
            whiteKnight = C64(0);
            whiteBishop = C64(0);
            whiteRook = C64(0);
            whiteQueen = C64(0);
            whiteKing = C64(0);
        } else {
            blackPawn = C64(0);
            blackKnight = C64(0);
            blackBishop = C64(0);
            blackRook = C64(0);
            blackQueen = C64(0);
            blackKing = C64(0);
        }
    }
 
    void updatePawn(Bitboards& bitboards, bool color) {
        if (color) {
            U64 pawn_bitboard = bitboards.whitePawn;
            whitePawn = ((pawn_bitboard & 0x7F7F7F7F7F7F7F7F) << 9) | ((pawn_bitboard & 0xFEFEFEFEFEFEFEFE) << 7);

        } else {
            //Same thing but modified for black
            U64 pawn_bitboard = bitboards.blackPawn; 
            blackPawn = ((pawn_bitboard & 0x7F7F7F7F7F7F7F7F) >> 7) | ((pawn_bitboard & 0xFEFEFEFEFEFEFEFE) >> 9);

        }
    }

    void updateKnight(Bitboards& bitboards, bool color) {
        if (color) {
            whiteKnight = C64(0);
            U64 knight_bitboard = bitboards.whiteKnight;
            if (knight_bitboard) do {
                whiteKnight |= knight_masks[bitScanForward(knight_bitboard)];
            } while (knight_bitboard &= knight_bitboard-1);
        } else {
            blackKnight = C64(0);
            U64 knight_bitboard = bitboards.blackKnight;
            if (knight_bitboard) do {
                blackKnight |= knight_masks[bitScanForward(knight_bitboard)];
            } while (knight_bitboard &= knight_bitboard-1);
        }
    }

    void updateBishop(Bitboards& bitboards, bool color) {
        U64 occupancy_wo_king = bitboards.occupancy ^ (color ? bitboards.blackKing : bitboards.whiteKing);
        if (color) {
            whiteBishop = C64(0);
            U64 bishop_bitboard = bitboards.whiteBishop;
            if (bishop_bitboard) do {
                U64 ls1b_bb = bishop_bitboard & -bishop_bitboard;
                U64 pos = bitScanForward(bishop_bitboard);
                whiteBishop |= sliding_moves(occupancy_wo_king, bishop_masks_diag1[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, bishop_masks_diag2[pos], ls1b_bb);
            } while (bishop_bitboard &= bishop_bitboard-1);
        } else {
            blackBishop = C64(0);
            U64 bishop_bitboard = bitboards.blackBishop;
            if (bishop_bitboard) do {
                U64 ls1b_bb = bishop_bitboard & -bishop_bitboard;
                U64 pos = bitScanForward(bishop_bitboard);
                blackBishop |= sliding_moves(occupancy_wo_king, bishop_masks_diag1[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, bishop_masks_diag2[pos], ls1b_bb);
            } while (bishop_bitboard &= bishop_bitboard-1);
        }
    }

    void updateRook(Bitboards& bitboards, bool color) {
        U64 occupancy_wo_king = bitboards.occupancy ^ (color ? bitboards.blackKing : bitboards.whiteKing);
        if (color) {
            whiteRook = C64(0);
            U64 rook_bitboard = bitboards.whiteRook;
            if (rook_bitboard) do {
                U64 ls1b_bb = rook_bitboard & -rook_bitboard;
                U64 pos = bitScanForward(rook_bitboard);
                whiteRook |= sliding_moves(occupancy_wo_king, rook_masks_vertical[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, rook_masks_horizontal[pos], ls1b_bb);
            } while (rook_bitboard &= rook_bitboard-1);
        } else {
            blackRook = C64(0);
            U64 rook_bitboard = bitboards.blackRook;
            if (rook_bitboard) do {
                U64 ls1b_bb = rook_bitboard & -rook_bitboard;
                U64 pos = bitScanForward(rook_bitboard);
                blackRook |= sliding_moves(occupancy_wo_king, rook_masks_vertical[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, rook_masks_horizontal[pos], ls1b_bb);
            } while (rook_bitboard &= rook_bitboard-1);
        }
    }

    void updateQueen(Bitboards& bitboards, bool color) {
        U64 occupancy_wo_king = bitboards.occupancy ^ (color ? bitboards.blackKing : bitboards.whiteKing);
        if (color) {
            whiteQueen = C64(0);
            U64 queen_bitboard = bitboards.whiteQueen;
            if (queen_bitboard) do {
                U64 ls1b_bb = queen_bitboard & -queen_bitboard;
                U64 pos = bitScanForward(queen_bitboard);
                whiteQueen |= sliding_moves(occupancy_wo_king, rook_masks_vertical[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, rook_masks_horizontal[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, bishop_masks_diag1[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, bishop_masks_diag2[pos], ls1b_bb);
            } while (queen_bitboard &= queen_bitboard-1);
        } else {
            blackQueen = C64(0);
            U64 queen_bitboard = bitboards.blackQueen;
            if (queen_bitboard) do {
                U64 ls1b_bb = queen_bitboard & -queen_bitboard;
                U64 pos = bitScanForward(queen_bitboard);
                blackQueen |= sliding_moves(occupancy_wo_king, rook_masks_vertical[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, rook_masks_horizontal[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, bishop_masks_diag1[pos], ls1b_bb) |
                    sliding_moves(occupancy_wo_king, bishop_masks_diag2[pos], ls1b_bb);
            } while (queen_bitboard &= queen_bitboard-1);
        }
    }

    void updateKing(int king_pos, Bitboards& bitboards, bool color) {
        color ? whiteKing : blackKing = king_masks[king_pos];
    }

};
