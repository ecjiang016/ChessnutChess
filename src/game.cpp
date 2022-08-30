#include "game.h"

void Chess::setFen(std::string fen) {
    int set_space = 56;

    //Reset all bitboards to empty
    for (int i = 0; i < 15; i++) {
        bitboards[i] = Bitboard(0);
    }

    //Reset mailbox to empty
    for (int i = 0; i < 64; i++) {
        mailbox[i] = NoPiece;
    }

    for (int i = 0; i < fen.size(); i++) {
        switch (fen[i]) {
            case 'P':
                bitboards[WhitePawn] |= get_single_bitboard(set_space);
                mailbox[set_space] = WhitePawn;
                break;
            case 'p':
                bitboards[BlackPawn] |= get_single_bitboard(set_space);
                mailbox[set_space] = BlackPawn;
                break;
            case 'N':
                bitboards[WhiteKnight] |= get_single_bitboard(set_space);
                mailbox[set_space] = WhiteKnight;
                break;
            case 'n':
                bitboards[BlackKnight] |= get_single_bitboard(set_space);
                mailbox[set_space] = BlackKnight;
                break;
            case 'B':
                bitboards[WhiteBishop] |= get_single_bitboard(set_space);
                mailbox[set_space] = WhiteBishop;
                break;
            case 'b':
                bitboards[BlackBishop] |= get_single_bitboard(set_space);
                mailbox[set_space] = BlackBishop;
                break;
            case 'R':
                bitboards[WhiteRook] |= get_single_bitboard(set_space);
                mailbox[set_space] = WhiteRook;
                break;
            case 'r':
                bitboards[BlackRook] |= get_single_bitboard(set_space);
                mailbox[set_space] = BlackRook;
                break;
            case 'Q':
                bitboards[WhiteQueen] |= get_single_bitboard(set_space);
                mailbox[set_space] = WhiteQueen;
                break;
            case 'q':
                bitboards[BlackQueen] |= get_single_bitboard(set_space);
                mailbox[set_space] = BlackQueen;
                break;
            case 'K':
                bitboards[WhiteKing] |= get_single_bitboard(set_space);
                mailbox[set_space] = WhiteKing;
                break;
            case 'k':
                bitboards[BlackKing] |= get_single_bitboard(set_space);
                mailbox[set_space] = BlackKing;
                break;
            case '/':
                set_space -= 17; //Wrap to the next row
                break;
            default:
                set_space += (int)fen[i] - 49; //Subtract 48 cause of ASCII and -1 to counteract the set_space++ later
        }
        set_space++;
    }

}

std::string Chess::getFen() const {
    std::string fen;
    char piece_char;
    int space_counter = 0;

    int set_space = 71; //Keep track of mailbox indices which are different from how fen indexes the board
                        //Should become 56 which is the index for the top left of the chess board after passing through the set_space update

    while (set_space != 7) {
        //Advance set_space and wrap to next row if needed
        set_space++;
        if (set_space % 8 == 0) {
            set_space -= 16;

            //The number of spaces neededs to be added before ending a row
            if (space_counter != 0) {
                fen += std::to_string(space_counter);
                space_counter = 0;
            }

            if (set_space != 56) { //Stops it from adding a "/" at the beginning of the fen
                fen += '/'; //End of row is marked with a "/"
            }       
        }

        Piece piece = mailbox[set_space];

        if (piece == NoPiece) {
            space_counter++;
            continue;
        }

        switch (getPieceType(piece)) {
            case Pawn:
                piece_char = 'P';
                break;
            case Knight:
                piece_char = 'N';
                break;
            case Bishop:
                piece_char = 'B';
                break;
            case Rook:
                piece_char = 'R';
                break;
            case Queen:
                piece_char = 'Q';
                break;
            case King:
                piece_char = 'K';
                break;
        }

        if (space_counter != 0) {
            fen += std::to_string(space_counter);
            space_counter = 0;
        }

        fen += piece_char + (32 * (int)getPieceColor(piece)); // Add 32 to convert letter to lowercase
    }

    return fen;
    
}

void Chess::print() const {
    std::string fen;
    char piece_char;
    int row = 9;
    int set_space = 71; //Keep track of mailbox indices which are different from how fen indexes the board
                        //Should become 56 which is the index for the top left of the chess board after passing through the set_space update

    while (set_space != 7) {
        //Advance set_space and wrap to next row if needed
        set_space++;
        if (set_space % 8 == 0) {
            set_space -= 16;
            row--;
            if (set_space != 56) {
                std::cout << " |\n   + - + - + - + - + - + - + - + - +\n " << row;
            } else {
                std::cout << "\n   + - + - + - + - + - + - + - + - +\n 8";
            }
        }

        Piece piece = mailbox[set_space];

        std::cout << " | ";

        if (piece == NoPiece) {
            std::cout << '.';
            continue;
        }

        switch (getPieceType(piece)) {
            case Pawn:
                piece_char = 'P';
                break;
            case Knight:
                piece_char = 'N';
                break;
            case Bishop:
                piece_char = 'B';
                break;
            case Rook:
                piece_char = 'R';
                break;
            case Queen:
                piece_char = 'Q';
                break;
            case King:
                piece_char = 'K';
                break;
        }

        std::cout << char((int)piece_char + 32 * (int)getPieceColor(piece)); // Add 32 to convert letter to lowercase
    }

    std::cout << " |\n   + - + - + - + - + - + - + - + - +\n";
    std::cout << "     a   b   c   d   e   f   g   h\n" << std::endl;
}