#include "moves.h"
#include <cstdlib>
#include "fen.h"
#include "tables.h"

class Chess {
  public:
    bool color;
    Bitboards bitboards;

  private:
    bool check;
    attackTable attack_table;
    U64 move_mask;
    U64 pinned_pieces;

    void updateCheckAndPins(Move last_move) {
        U64 king_bb = color ? bitboards.whiteKing : bitboards.blackKing;
        int king_idx = bitScanForward(king_bb);
        check = color ? bitboards.whiteKing & attack_table.getBlackAttacks() : bitboards.blackKing & attack_table.getWhiteAttacks();
        move_mask = C64(0xffffffffffffffff);
        pinned_pieces = C64(0);

        int checks = 0; //For keeping track of checks on the king

        U64 bishop_pot_check = (bishop_masks_diag1[king_idx] | bishop_masks_diag2[king_idx]) &
                (color ? (bitboards.blackBishop | bitboards.blackQueen) : (bitboards.whiteBishop | bitboards.whiteQueen));
        U64 rook_pot_check = (rook_masks_horizontal[king_idx] | rook_masks_vertical[king_idx]) &
            (color ? (bitboards.blackRook | bitboards.blackQueen) : (bitboards.whiteRook | bitboards.whiteQueen));

        if (check) {
            move_mask = C64(0);
        }

        if (bishop_pot_check) do {
            int piece_idx = bitScanForward(bishop_pot_check);
            U64 in_between_spaces = bitboards.occupancy & connecting_masks[king_idx][piece_idx];
            if (!(in_between_spaces)) { 
                //Empty so the piece is an attacker
                move_mask = connecting_masks[king_idx][piece_idx];
                checks++;
            } else if (((in_between_spaces & (in_between_spaces-1)) == 0) && (in_between_spaces & (color ? bitboards.occupancyWhite : bitboards.occupancyBlack))) {
                //There's one piece in between and it's a friendly piece
                pinned_pieces |= in_between_spaces;
            }
        } while (bishop_pot_check &= bishop_pot_check-1);
        
        if (rook_pot_check) do {
            int piece_idx = bitScanForward(rook_pot_check);
            U64 in_between_spaces = bitboards.occupancy & connecting_masks[king_idx][piece_idx];
            if (!(in_between_spaces)) { 
                //Empty so the piece is an attacker
                move_mask = connecting_masks[king_idx][piece_idx];
                checks++;
            } else if (((in_between_spaces & (in_between_spaces-1)) == 0) && (in_between_spaces & (color ? bitboards.occupancyWhite : bitboards.occupancyBlack))) {
                //There's one piece in between and it's a friendly piece
                pinned_pieces |= in_between_spaces;
            }
        } while (rook_pot_check &= rook_pot_check-1);

        if (check) {

            //Find where the check is coming from
            //The only way for a double check is a discovered attack with a sliding piece as one of the checkers
            //It's also not possible for a double check with 2 of the same slider piece type (diagonal & diagonal or vertical/horizontal & vertical/horizontal)
            //Pawns cannot also be part of a double check
            
            if (checks == 1 && last_move.piece == Knight) {
                move_mask = C64(0); //Double check, must move king as there's no way to block both or capture both
            } else if (checks == 2) {
                move_mask = C64(0); //Double check, must move king as there's no way to block both or capture both
            } else {
                move_mask |= C64(1) << last_move.new_c; //Can also capture the checker piece
            }

        }

    }

    void updateAttackTableByPiece(Piece piece) {
        switch (piece) {
            case None:
                break;
            case Pawn:
                attack_table.updatePawn(bitboards, color);
                break;
            case Knight:
                attack_table.updateKnight(bitboards, color);
                break;
            case Bishop:
                attack_table.updateBishop(bitboards, color);
                break;
            case Rook:
                attack_table.updateRook(bitboards, color);
                break;
            case Queen:
                attack_table.updateQueen(bitboards, color);
                break;
            case King:
                attack_table.updateKing(bitScanForward(color ? bitboards.whiteKing : bitboards.blackKing), bitboards, color);
                break;
        }
    }

  public:
    Chess() {
        color = 1;
        FEN(starting_pos, bitboards);
        move_mask = C64(0xffffffffffffffff);
        pinned_pieces = C64(0);
    }

    bool inCheck() {
        return (color ? bitboards.whiteKing & attack_table.getBlackAttacks() : bitboards.blackKing & attack_table.getWhiteAttacks()) != 0;
    }

    void loadFen(std::string fen) {
        bitboards.clearBitboards();
        attack_table.clearTables(color);
        attack_table.clearTables(!color);
        FEN(fen, bitboards);
        attack_table.updatePawn(bitboards, !color);
        attack_table.updateKnight(bitboards, !color);
        attack_table.updateBishop(bitboards, !color);
        attack_table.updateRook(bitboards, !color);
        attack_table.updateQueen(bitboards, !color);
        attack_table.updateKing(bitScanForward(color ? bitboards.blackKing : bitboards.whiteKing), bitboards, !color);
        Move empty;
        updateCheckAndPins(empty);
    }

    void makeMove(Move move) {
        U64* piece_bitboard = bitboards.getBitboard(color, move.piece);
        *piece_bitboard &= ~(C64(1) << move.old_c); //Clear old position
        *piece_bitboard |= C64(1) << move.new_c; //Set piece in the piece's new position
        
        //Clear bit that's being moved to if there is a capture
        if (move.capture != None) {
            *bitboards.getBitboard(!color, move.capture) &= ~(C64(1) << move.new_c);
        }
        
        
        if (((abs(move.new_c - move.old_c) == 7) || (abs(move.new_c - move.old_c) == 9)) && (move.piece == Pawn) && (move.capture == None)) {
            //There is an en passant and the caputred pawn needs to be removed
            (color ? bitboards.blackPawn : bitboards.whitePawn) ^= bitboards.en_passant;
        }
        //Set en passant board/mask
        bitboards.en_passant = C64(0); //Reset bitboard
        //Set bit if there is a double pawn push
        if ((abs(move.new_c - move.old_c) == 16) && (move.piece == Pawn)) {
            bitboards.en_passant |= C64(1) << move.new_c;
        }

        //Updates
        bitboards.updateOccupancy();
        //updateAttackTableByPiece(move.piece);
        attack_table.updatePawn(bitboards, color);
        attack_table.updateKnight(bitboards, color);
        attack_table.updateBishop(bitboards, color);
        attack_table.updateRook(bitboards, color);
        attack_table.updateQueen(bitboards, color);
        attack_table.updateKing(bitScanForward(color ? bitboards.whiteKing : bitboards.blackKing), bitboards, color);

        color = !color;
        //Prepare for the next move
        updateCheckAndPins(move);
        attack_table.clearTables(color);
    }

    void unmakeMove(Move move) {
        color = !color;

        U64* piece_bitboard = bitboards.getBitboard(color, move.piece);
        *piece_bitboard &= ~(C64(1) << move.new_c); //Clear old position
        *piece_bitboard |= C64(1) << move.old_c; //Set piece in the piece's new position
        
        if (move.capture != None) {
            *bitboards.getBitboard(!color, move.capture) |= C64(1) << move.new_c;
        }

        //Updates
        bitboards.updateOccupancy();
        updateAttackTableByPiece(move.piece);

        
    }

    std::vector<Move> allMoves() {
        std::vector<Move> moves;
        moves.reserve(128);
        U64 color_occupancy = color ? bitboards.occupancyWhite : bitboards.occupancyBlack;
        Bitboards* bb_ptr = &bitboards;

        PawnMoves(bitboards, moves, color, attack_table, move_mask, pinned_pieces);

        U64 knight_bitboard = (color ? bitboards.whiteKnight : bitboards.blackKnight) & ~pinned_pieces;
        if (knight_bitboard) do {
            KnightMoves(knight_bitboard & -knight_bitboard, color_occupancy, moves, color, attack_table, bb_ptr, move_mask);
        } while (knight_bitboard &= knight_bitboard-1);

        U64 bishop_bitboard = color ? bitboards.whiteBishop : bitboards.blackBishop;
        if (bishop_bitboard) do {
            BishopMoves(bishop_bitboard & -bishop_bitboard, bitboards.occupancy, color_occupancy, moves, color, attack_table, bb_ptr, move_mask, pinned_pieces);
        } while (bishop_bitboard &= bishop_bitboard-1);

        U64 rook_bitboard = color ? bitboards.whiteRook : bitboards.blackRook;
        if (rook_bitboard) do {
            RookMoves(rook_bitboard & -rook_bitboard, bitboards.occupancy, color_occupancy, moves, color, attack_table, bb_ptr, move_mask, pinned_pieces);
        } while (rook_bitboard &= rook_bitboard-1);

        U64 queen_bitboard = color ? bitboards.whiteQueen : bitboards.blackQueen;
        if (queen_bitboard) do {
            QueenMoves(queen_bitboard & -queen_bitboard, bitboards.occupancy, color_occupancy, moves, color, attack_table, bb_ptr, move_mask, pinned_pieces);
        } while (queen_bitboard &= queen_bitboard-1);
        
        if (inCheck()) {
            //For adding x-ray attacks that go through the king
            attack_table.updateBishop(bitboards, !color);
            attack_table.updateRook(bitboards, !color);
            attack_table.updateQueen(bitboards, !color);
        }
        

        KingMoves(color ? bitboards.whiteKing : bitboards.blackKing, color_occupancy, moves, color, attack_table, bb_ptr);

        return moves;
    }

};