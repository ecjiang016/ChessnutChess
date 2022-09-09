# distutils: language = c++

from libcpp.vector cimport vector
from libcpp.string cimport string
from libc.stdint cimport uint8_t
cimport cython
import numpy as np

cdef extern from "../src/game.h":
    enum Color:
        WHITE = 0
        BLACK = 1

    enum PieceType:
        NoPiece = 0
        Pawn = 1
        Knight = 2
        Bishop = 3
        Rook = 4
        Queen = 5
        King = 6

    enum Piece:
        WhitePawn = 1
        WhiteKnight = 2
        WhiteBishop = 3
        WhiteRook = 4
        WhiteQueen = 5
        WhiteKing = 6
        BlackPawn = 9
        BlackKnight = 10
        BlackBishop = 11
        BlackRook = 12
        BlackQueen = 13
        BlackKing = 14

    enum Flag:
        QUIET = 4096
        CAPTURE = 4097

    cdef cppclass Move:
        Move() except +
        Move(uint8_t from_, uint8_t to) except +
        Move(uint8_t from_, uint8_t to, Flag flag) except +

        uint8_t from_ "from"()
        uint8_t to()
        string cUCI "UCI"()

    cdef cppclass cChess "Chess":
        cChess() except +
        cChess(string fen) except +

        vector[Move] getMoves_white "getMoves<WHITE>"()
        vector[Move] getMoves_black "getMoves<BLACK>"()
        void makeMove_white "makeMove<WHITE>"(Move move)
        void makeMove_black "makeMove<BLACK>"(Move move)
        void unmakeMove_white "unmakeMove<WHITE>"(Move move)
        void unmakeMove_black "unmakeMove<BLACK>"(Move move)
        vector[Piece] getMailbox()
        void csetFen "setFen"(string fen)

def coords_2D_to_1D(x, y):
    return (7-y)*8 + x

cdef class Chess:
    cdef cChess *cobj
    cdef Color color
    cdef Move last_move #For undoing moves
    def __cinit__(self):
        self.cobj = new cChess()
        self.color = WHITE
        self.last_move = Move()

    property color:
        def __set__(self, val):
            if val == 0:
                self.color = WHITE
            elif val == 1:
                self.color = BLACK

        def __get__(self):
            return int(self.color)

    def getAllMoves(self):
        move_list = []
        cdef vector[Move] allMoves = self.cobj.getMoves_white() if self.color == WHITE else self.cobj.getMoves_black()
        cdef int i
        cdef Move move

        for i in range(allMoves.size()):
            move = allMoves[i]
            move_list.append((<bytes>move.cUCI()).decode("utf-8"))
        
        return move_list

    def pieceMoves(self, piece_x, piece_y):
        move_list = []
        piece_coord = coords_2D_to_1D(piece_x, piece_y)
        cdef vector[Move] allMoves = self.cobj.getMoves_white() if self.color == WHITE else self.cobj.getMoves_black()
        cdef Move move
        for i in range(allMoves.size()):
            move = allMoves[i]
            if move.from_() == piece_coord:
                move_list.append(move.to())

        return move_list

    def checkLegalMove(self, old_x, old_y, new_x, new_y):
        old_c = coords_2D_to_1D(old_x, old_y)
        new_c = coords_2D_to_1D(new_x, new_y)
        cdef vector[Move] allMoves = self.cobj.getMoves_white() if self.color == WHITE else self.cobj.getMoves_black()
        cdef Move move
        for i in range(allMoves.size()):
            move = allMoves[i]
            if move.from_() == old_c and move.to() == new_c:
                return True

        return False

    def makeMove(self, old_x, old_y, new_x, new_y):
        old_c = coords_2D_to_1D(old_x, old_y)
        new_c = coords_2D_to_1D(new_x, new_y)
        cdef vector[Move] allMoves = self.cobj.getMoves_white() if self.color == WHITE else self.cobj.getMoves_black()
        cdef Move move
        for i in range(allMoves.size()):
            move = allMoves[i]
            if move.from_() == old_c and move.to() == new_c:
                break

        if self.color == WHITE:
            self.cobj.makeMove_white(move)
            self.color = BLACK
        else:
            self.cobj.makeMove_black(move)
            self.color = WHITE

        self.last_move = move #Cache for unmake

    def unmakeMove(self):
        if self.color == WHITE:
            self.cobj.unmakeMove_black(self.last_move)
            self.color = BLACK
        else:
            self.cobj.unmakeMove_white(self.last_move)
            self.color = WHITE

    #@cython.boundscheck(False)  # Deactivate bounds checking
    #@cython.wraparound(False)
    def createBoard(self):
        cdef int [:] board = np.zeros(64, dtype="int32") 
        cdef vector[Piece] cboard = self.cobj.getMailbox()

        for square in range(64):
            if int(cboard[square]) <= 6:
                board[square] = int(cboard[square])
            else:
                board[square] = -int(cboard[square]) + 8

        return np.flipud(np.array(board).reshape(8, 8))

    def setFen(self, fen):
        self.cobj.csetFen(fen.encode("utf-8"))