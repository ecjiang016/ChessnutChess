#include "bits.h"

const std::string row_str = "  + - + - + - + - + - + - + - + - +";

void print_bitboard(Bitboard bitboard) {
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

