import numpy as np

FILE = "./src/game/masks.h"

def convert_to_hex_bitboard(board):
    continuous_board = np.flipud(board).ravel()
    decimal = sum([digit*(2**power) for power, digit in enumerate(continuous_board)])
    return hex(int(decimal))

new_pos = np.fliplr(np.flipud(np.arange(64).reshape(8, 8))).ravel()
def convert_pos(pos):
    """
    The Python code uses a board that starts on the top left and counts right and down.
    The C++ code uses a board that starts on the bottom left and counts right and up.
    This function converts from the Python one to the C++ one
    """
    return new_pos[pos]

def compute_rook_masks():
    directions = [-1, 1, -8, 8]
    with open(FILE, "a") as f:
        f.write("U64 rook_masks_horizontal[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, 7-x, y, 7-y]
            board = np.zeros(64)
            for d in range(2):
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1

            board[pos] = 1
            f.write("    " + convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")

        f.write("U64 rook_masks_vertical[64] = {\n")
        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, 7-x, y, 7-y]
            board = np.zeros(64)
            for d in range(2, 4):
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1
            board[pos] = 1
            f.write("    " + convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.close()
        
def compute_bishop_masks():
    directions = [-9, 9, -7, 7]
    with open(FILE, "a") as f:
        f.write("U64 bishop_masks_diag1[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [min(y, x), min(7-y, 7-x), min(y, 7-x), min(7-y, x)]
            board = np.zeros(64)
            f.write("    ")
            for d in range(2):
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1
            f.write(convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")

        f.write("U64 bishop_masks_diag2[64] = {\n")
        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [min(y, x), min(7-y, 7-x), min(y, 7-x), min(7-y, x)]
            board = np.zeros(64)
            f.write("    ")
            for d in range(2, 4):
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1
            f.write(convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.close()

def compute_knight_masks():
    directions = [-10, -17, -15, -6, 10, 17, 15, 6]
    with open(FILE, "a") as f:
        f.write("U64 knight_masks[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [(x > 1) * (y > 0), (x > 0) * (y > 1), (x < 7) * (y > 1), (x < 6) * (y > 0), (x < 6) * (y < 7), (x < 7) * (y < 6), (x > 0) * (y < 6), (x > 1) * (y < 7)]
            board = np.zeros(64)
            f.write("    ") #Indent
            for d in range(8):
                if spaces_to_edge[d]: #Not over edge
                    board[pos + directions[d]] = 1

            f.write(convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.close()

def compute_king_masks():
    directions = [-1, -9, -8, -7, 1, 9, 8, 7, -2, 2]
    with open(FILE, "a") as f:
        f.write("U64 king_masks[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, min(y, x), y, min(y, 7-x), 7-x, min(7-y, 7-x), 7-y, min(7-y, x)]
            board = np.zeros(64)
            f.write("    ") #Indent
            for d in range(8):
                if spaces_to_edge[d]: #Not over edge
                    board[pos + directions[d]] = 1

            #if pos == 4 or pos == 60: #Can castle:
            #    board[x-2] = 1
            #    board[x+2] = 1

            f.write(convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.close()

if __name__ == '__main__':
    #Clear the file
    with open(FILE, "w") as f:
        f.truncate(0)
        f.close()

    with open(FILE, "a") as f:
        f.write("typedef unsigned long long U64;\n\n")
        f.close()

    compute_rook_masks()
    compute_bishop_masks()
    compute_knight_masks()
    compute_king_masks()
    print("Masks Generated")