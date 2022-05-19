#pragma once
#include "utils.h"
#include <iostream>
#include <string>

std::string starting_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

void FEN(std::string fen_string, Bitboards& bitboards) {
    int set_space = 56;
    for (std::string::size_type i = 0; i < fen_string.size(); ++i) {
        switch (fen_string[i]) {
            case *"P":
                bitboards.whitePawn |= C64(1) << set_space;
                break;
            case *"p":
                bitboards.blackPawn  |= C64(1) << set_space;
                break;
            case *"N":
                bitboards.whiteKnight |= C64(1) << set_space;
                break;
            case *"n":
                bitboards.blackKnight |= C64(1) << set_space;
                break;
            case *"B":
                bitboards.whiteBishop |= C64(1) << set_space;
                break;
            case *"b":
                bitboards.blackBishop |= C64(1) << set_space;
                break;
            case *"R":
                bitboards.whiteRook |= C64(1) << set_space;
                break;
            case *"r":
                bitboards.blackRook |= C64(1) << set_space;
                break;
            case *"Q":
                bitboards.whiteQueen |= C64(1) << set_space;
                break;
            case *"q":
                bitboards.blackQueen |= C64(1) << set_space;
                break;
            case *"K":
                bitboards.whiteKing |= C64(1) << set_space;
                break;
            case *"k":
                bitboards.blackKing |= C64(1) << set_space;
                break;
            case *"/":
                set_space -= 17; //Wrap to the next row
                break;
            default:
                set_space += (int)fen_string[i] - 49; //Subtract 48 cause of ASCII and -1 to counteract the set_space++ later
        }
        set_space++; 
    }
    
    bitboards.updateOccupancy();
}