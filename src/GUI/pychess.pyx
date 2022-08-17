# distutils: language = c++

from libcpp.vector cimport vector
from libcpp.string cimport string
from cython.operator cimport dereference
cimport cython
import numpy as np

def coords_2D_to_1D(x, y):
    return (7-y)*8 + x

cdef extern from "../game/utils.h":
    enum Piece:
        Pawn = 1
        Knight = 2
        Bishop = 3
        Rook = 4
        Queen = 5
        King = 6

    unsigned long long check_bit(unsigned long long, int)
    int bitScanForward(unsigned long long bb)

    cdef cppclass Bitboards:
        Bitboards() except +
        unsigned long long whitePawn, blackPawn, whiteKnight, blackKnight, whiteBishop, blackBishop
        unsigned long long whiteRook, blackRook, whiteQueen, blackQueen, whiteKing, blackKing

        unsigned long long occupancy, occupancyWhite, occupancyBlack
        unsigned long long attackWhite, attackBlack

        unsigned long long en_passant

        unsigned long long * getBitboard(bint color, Piece piece)
        unsigned long long * getBitboard(bint color, int piece)

cdef extern from "../game/moves.h":
    cdef cppclass Move:
        Move(int, int, Piece) except +
        Move(string, Piece) except +
        int old_c
        int new_c
        Piece piece
        string UCI()

cdef extern from "../game/game.cpp":
    cdef cppclass Chess:
        Chess() except +
        bint color
        Bitboards bitboards
        vector[Move] allMoves()
        void makeMove(Move move)
        void loadFen(string fen)

cdef class pyChess:
    cdef Chess *cobj
    def __cinit__(self):
        self.cobj = new Chess()

    property color:
        def __get__(self):
            return self.cobj.color
        def __set__(self, bint color):
            self.cobj.color = color

    def loadFen(self, fen):
        self.cobj.loadFen(fen.encode('utf-8'))

    def pieceMoves(self, piece_x, piece_y):
        move_list = []
        piece_coord = coords_2D_to_1D(piece_x, piece_y)
        cdef vector[Move] allMoves = self.cobj.allMoves()
        cdef Move* move
        for i in range(allMoves.size()):
            move = &allMoves[i]
            if move.old_c == piece_coord:
                move_list.append(move.new_c)

        return move_list

    def checkLegalMove(self, old_x, old_y, new_x, new_y):
        old_c = coords_2D_to_1D(old_x, old_y)
        new_c = coords_2D_to_1D(new_x, new_y)
        cdef vector[Move] allMoves = self.cobj.allMoves()
        cdef Move* move
        for i in range(allMoves.size()):
            move = &allMoves[i]
            if move.old_c == old_c and move.new_c == new_c:
                return True

        return False

    def makeMove(self, old_x, old_y, new_x, new_y):
        old_c = coords_2D_to_1D(old_x, old_y)
        new_c = coords_2D_to_1D(new_x, new_y)
        cdef vector[Move] allMoves = self.cobj.allMoves()
        cdef Move* move
        for i in range(allMoves.size()):
            move = &allMoves[i]
            if move.old_c == old_c and move.new_c == new_c:
                break

        self.cobj.makeMove(dereference(move))

    @cython.boundscheck(False)  # Deactivate bounds checking
    @cython.wraparound(False)
    def createBoard(self):
        cdef int [:] board = np.zeros(64, dtype="int32") 
        cdef int piece
        cdef unsigned long long bitboard

        for piece in range(1, 7):
            bitboard = dereference(self.cobj.bitboards.getBitboard(1, piece))
            while bitboard:
                board[bitScanForward(bitboard)] = piece
                bitboard &= bitboard-1

            bitboard = dereference(self.cobj.bitboards.getBitboard(0, piece))
            while bitboard:
                board[bitScanForward(bitboard)] = -piece
                bitboard &= bitboard-1

        return np.flipud(np.array(board).reshape(8, 8))

            