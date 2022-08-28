#include "game.h"

constexpr inline Bitboard Chess::get_bitboard(PieceType piece, Color color) const {
	return bitboards[makePiece(piece, color)];
}

inline std::vector<Piece> Chess::getMailbox() const {
    return std::vector<Piece>(mailbox, mailbox + 64);
}

constexpr Color Chess::setFen(char* fen) {
    int set_space = 56;

    int i = 0;
    while (fen[i] != ' ') {
        switch (fen[i]) {
            case 'P':
                this->bitboards[WhitePawn] |= get_single_bitboard(set_space);
                break;
            case 'p':
                this->bitboards[BlackPawn] |= get_single_bitboard(set_space);
                break;
            case 'N':
                this->bitboards[WhiteKnight] |= get_single_bitboard(set_space);
                break;
            case 'n':
                this->bitboards[BlackKnight] |= get_single_bitboard(set_space);
                break;
            case 'B':
                this->bitboards[WhiteBishop] |= get_single_bitboard(set_space);
                break;
            case 'b':
                this->bitboards[BlackBishop] |= get_single_bitboard(set_space);
                break;
            case 'R':
                this->bitboards[WhiteRook] |= get_single_bitboard(set_space);
                break;
            case 'r':
                this->bitboards[BlackRook] |= get_single_bitboard(set_space);
                break;
            case 'Q':
                this->bitboards[WhiteQueen] |= get_single_bitboard(set_space);
                break;
            case 'q':
                this->bitboards[BlackQueen] |= get_single_bitboard(set_space);
                break;
            case 'K':
                this->bitboards[WhiteKing] |= get_single_bitboard(set_space);
                break;
            case 'k':
                this->bitboards[BlackKing] |= get_single_bitboard(set_space);
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
    
    return (fen[i + 1] == 'w' || fen[i + 1] == 'W') ? WHITE : BLACK; //White or black indicator is one after the space

}