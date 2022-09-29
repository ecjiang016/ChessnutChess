#include "game.h"

Color Chess::setFen(std::string fen) {
    int set_space = 56;

    //Reset all bitboards to empty
    for (int i = 0; i < 15; i++) {
        bitboards[i] = Bitboard(0);
    }

    //Reset mailbox to empty
    for (int i = 0; i < 64; i++) {
        mailbox[i] = NoPiece;
    }

    uint8_t i = 0;
    while (fen[i] != ' ') { //The piece positions
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
        i++;
    }

    i++; //Skip the first space
    //Other fen information
    //color
    Color color = fen[i] == 'w' ? WHITE : BLACK;
    i += 2;

    //Castling rights
    bool castle_WK = false;
    bool castle_WQ = false;
    bool castle_Bk = false;
    bool castle_Bq = false;
    while (fen[i] != ' ') {
        switch (fen[i]) {
            case 'K':
                castle_WK = true;
                break;
            case 'Q':
                castle_WQ = true;
                break;
            case 'k':
                castle_Bk = true;
                break;
            case 'q':
                castle_Bq = true;
                break;
        }
        i++;
    }

    //En passant square
    int en_passant_square = 0; //0 for no en passant square since there can't possibly be an en passant there
    i++;
    if (fen[i] != '-') {
        en_passant_square = string_to_index(fen.substr(i, 2));
    }

    //We do not keep track of halfmoves
    i += 2;
    while (fen[i] != ' ') {
        i++;
    }

    //Fullmove number gets converted and sets the depth
    //The space after halfmove and fullmove is taken into consideration here
    if (fen.size() - i > i + 1) { //Sometimes the fen doesn't provide move numbers and stoi will crash stuff if it's not there
        int fullmove = std::stoi(fen.substr(i + 1, fen.size() - i)); //Grabs the rest of the fen
        this->depth = (fullmove - 1) * 2 + color; //Adding one if black plays next
    }

    //Now that depth is set, the history info can be put into the right index
    this->history[depth] = History();

    //Set castling status of king
    //If either side castling is possible, that means king hasn't moved and its bit needs to be set to 0
    if (castle_WK | castle_WQ) {
        this->history[depth].castling &= ~Bitboard(0b10000);
    } else {
        this->history[depth].castling |= Bitboard(0b10000);
    }
    if (castle_Bk | castle_Bq) {
        this->history[depth].castling &= ~(Bitboard(0b10000) << 56);
    } else {
        this->history[depth].castling |= Bitboard(0b10000) << 56;
    }

    //Set castling status of rooks
    //Bit set to 0 if castling can be done on that rook, otherwise it's set to 0
    if (castle_WK) {
        this->history[depth].castling &= ~Bitboard(0b10000000);
        
    } else {
        this->history[depth].castling |= Bitboard(0b10000000);
    }
    if (castle_WQ) {
        this->history[depth].castling &= ~Bitboard(1);
        
    } else {
        this->history[depth].castling |= Bitboard(1);
    }
    if (castle_Bk) {
        this->history[depth].castling &= ~(Bitboard(0b10000000) << 56);
        
    } else {
        this->history[depth].castling |= Bitboard(0b10000000) << 56;
    }
    if (castle_Bq) {
        this->history[depth].castling &= ~(Bitboard(1) << 56);
        
    } else {
        this->history[depth].castling |= Bitboard(1) << 56;
    }
    

    //Put en passant square in history
    if (en_passant_square) {
        this->history[depth].en_passant_square = en_passant_square;
    }

    //History.capture cannot be set as fen doesn't provide the information for it
    //This also means that undoing past where the fen was set would be bugged
    
    return color; //Color isn't being kept track of by the class so the code using it needs to get the color back from the fen reading

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