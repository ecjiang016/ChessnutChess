#include "bits.h"

const std::string row_str = "  + - + - + - + - + - + - + - + - +";
void print_bitboards(Bitboards& bitboards) {
    int piece_at_pos;
    uint64_t bitboard_array [12] = {
        bitboards.whitePawn,
        bitboards.blackPawn,
        bitboards.whiteKnight,
        bitboards.blackKnight,
        bitboards.whiteBishop,
        bitboards.blackBishop,
        bitboards.whiteRook,
        bitboards.blackRook,
        bitboards.whiteQueen,
        bitboards.blackQueen,
        bitboards.whiteKing,
        bitboards.blackKing
    };

    std::cout << std::endl;
    for (int row = 7; row >= 0; row--) {
        std::cout << row_str << std::endl;
        std::cout << (row + 1);
        std::cout << " ";

        for (int column = 0; column < 8; column++) {
            std::cout << "| ";

            for (int i = 0; i <= 12; i++) {
                if (check_bit(bitboard_array[i], ((row*8) + column))) {
                    piece_at_pos = i;
                    break;
                }
                if (i == 12) {
                    piece_at_pos = 12;
                }
            }

            switch (piece_at_pos) {
                case 0:
                    std::cout << "P";
                    break;
                case 1:
                    std::cout << "p";
                    break;
                case 2:
                    std::cout << "N";
                    break;
                case 3:
                    std::cout << "n";
                    break;
                case 4:
                    std::cout << "B";
                    break;
                case 5:
                    std::cout << "b";
                    break;
                case 6:
                    std::cout << "R";
                    break;
                case 7:
                    std::cout << "r";
                    break;
                case 8:
                    std::cout << "Q";
                    break;
                case 9:
                    std::cout << "q";
                    break;
                case 10:
                    std::cout << "K";
                    break;
                case 11:
                    std::cout << "k"; 
                    break;
                case 12:
                    std::cout << ".";
                    break;
            }

            std::cout << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << row_str << std::endl;
    std::cout << "    a   b   c   d   e   f   g   h" << std::endl << std::endl;
    
};

void print_single_bitboard(uint64_t bitboard) {
    std::cout << std::endl;
    for (int row = 7; row >= 0; row--) {
        std::cout << row_str << std::endl;
        std::cout << (row + 1);
        std::cout << " ";
    
        for (int column = 0; column < 8; column++) {
            std::cout << "| ";

            if (check_bit(bitboard, ((row*8) + column))) {
                std::cout << "1";
            } else {
                std::cout << ".";
            }

            std::cout << " ";
        }
        std::cout << "|" << std::endl;
    }
    std::cout << row_str << std::endl;
    std::cout << "    a   b   c   d   e   f   g   h" << std::endl << std::endl;
    
};

