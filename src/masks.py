import numpy as np

FILE = "./src/masks.h"

def convert_to_hex_bitboard(board):
    #decimal = sum([digit*(2**power) for power, digit in enumerate(np.flipud(board.reshape(8, 8)).ravel())])
    decimal = int(''.join(map(str, np.fliplr(board.reshape(8, 8)).ravel().astype(str))), 2)
    return hex(decimal)

def convert_2d_to_1d(x, y):
    return (y*8) + x

new_pos = np.flipud(np.arange(64).reshape(8, 8)).ravel()
def convert_pos(pos):
    """
    The Python code uses a board that starts on the top left and counts right and down.
    The C++ code uses a board that starts on the bottom left and counts right and up.
    This function converts from the Python one to the C++ one
    """
    return int(new_pos[pos])

def compute_rook_masks():
    directions = [-1, 1, -8, 8]
    with open(FILE, "a") as f:
        f.write("const Bitboard rook_masks_horizontal[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, 7-x, y, 7-y]
            board = np.zeros(64, dtype=int)
            for d in range(2):
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1

            f.write("    " + convert_to_hex_bitboard(board))

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")

        f.write("const Bitboard rook_masks_vertical[64] = {\n")
        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, 7-x, y, 7-y]
            board = np.zeros(64, dtype=int)
            for d in range(2, 4):
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1

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
        f.write("const Bitboard bishop_masks_diag1[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [min(y, x), min(7-y, 7-x), min(y, 7-x), min(7-y, x)]
            board = np.zeros(64, dtype=int)
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

        f.write("const Bitboard bishop_masks_diag2[64] = {\n")
        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [min(y, x), min(7-y, 7-x), min(y, 7-x), min(7-y, x)]
            board = np.zeros(64, dtype=int)
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
        f.write("const Bitboard knight_masks[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [(x > 1) * (y > 0), (x > 0) * (y > 1), (x < 7) * (y > 1), (x < 6) * (y > 0), (x < 6) * (y < 7), (x < 7) * (y < 6), (x > 0) * (y < 6), (x > 1) * (y < 7)]
            board = np.zeros(64, dtype=int)
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
        f.write("const Bitboard king_masks[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, min(y, x), y, min(y, 7-x), 7-x, min(7-y, 7-x), 7-y, min(7-y, x)]
            board = np.zeros(64, dtype=int)
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

def compute_all_direction_masks():
    """All 8 directions (Queen move)"""
    directions = [-1, -9, -8, -7, 1, 9, 8, 7, -2, 2]
    with open(FILE, "a") as f:
        f.write("const Bitboard all_direction_masks[64] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, min(y, x), y, min(y, 7-x), 7-x, min(7-y, 7-x), 7-y, min(7-y, x)]
            board = np.zeros(64, dtype=int)
            f.write("    ") #Indent
            for d in range(8):
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

def compute_directional_masks():
    """Create rays for the 8 directions independently"""
    directions = [-1, -9, -8, -7, 1, 9, 8, 7, -2, 2]
    with open(FILE, "a") as f:
        f.write("const Bitboard directional_masks[64][8] = {\n")

        for pypos in range(64):
            pos = convert_pos(pypos)
            x = pos % 8
            y = pos // 8
            spaces_to_edge = [x, min(y, x), y, min(y, 7-x), 7-x, min(7-y, 7-x), 7-y, min(7-y, x)]
            f.write("    {\n") #Indent
            for d in range(8):
                board = np.zeros(64, dtype=int)
                f.write("    ")
                for space in range(1, spaces_to_edge[d]+1):
                    check_pos = space * directions[d] + pos
                    board[check_pos] = 1

                f.write(convert_to_hex_bitboard(board))
                if d != 7:
                    f.write(",\n")
                else:
                    f.write("\n    }")

            if pypos != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.close()

def compute_connecting_masks():
    """
    Connects two positions on the board from the first to the second, not including endpoints.

    If the two positions cannot be connected, it will return all zeros.
    """

    with open(FILE, "a") as f:
        f.write("const Bitboard connecting_masks[64][64] = {\n")

        for pypos1 in range(64):
            pos1 = convert_pos(pypos1)
            x1 = pos1 % 8
            y1 = pos1 // 8
            f.write("    {\n") #Indent

            for pypos2 in range(64):
                pos2 = convert_pos(pypos2)
                x2 = pos2 % 8
                y2 = pos2 // 8
                
                board = np.zeros(64, dtype=int)
                if pos1 == pos2:
                    pass

                elif x1 == x2: #Vertical mask
                    for s in range(1, abs(y2 - y1)):
                        board[s * np.sign(y2 - y1) * 8 + pos1] = 1

                elif y1 == y2: #Horizontal mask
                    for s in range(1, abs(x2 - x1)):
                        board[s * np.sign(x2 - x1) + pos1] = 1

                elif (y1-y2)/(x2-x1) == 1: #Diagonal, positive slope
                    if (y1 - y2) >= 1: #Up right
                        for s in range(1, abs(y1 - y2)):
                            board[s * -7 + pos1] = 1

                    else: #Down left
                        for s in range(1, abs(y1 - y2)):
                            board[s * 7 + pos1] = 1


                elif (y1-y2)/(x2-x1) == -1: #Diagonal, negative slope
                    if (y1 - y2) >= 1: #Up left
                        for s in range(1, abs(y1 - y2)):
                            board[s * -9 + pos1] = 1

                    else: #Down right
                        for s in range(1, abs(y1 - y2)):
                            board[s * 9 + pos1] = 1
                
                else: #Fail safe idk why there really isn't a point cause there's no conceivable way of it ever getting here and the board not being full of zeros but since I wrote it already why not keep it for absolutely no reason but to make myself feel a little better and worse at the same time since it shouldn't break but then also it shouldn't even get to the breaking point in the first place. Well now the only reason why this exists is to host this abusdly long comment that's kinda funny at the time that I'm writing this but might prove to be a little bothersome later.
                    board = np.zeros(64, dtype=int)

                f.write("    ")
                f.write(convert_to_hex_bitboard(board)) 
                if pypos2 != 63: #Last item in list doesn't need a comma
                    f.write(", ")

                    if pypos2 % 8 == 7: #Group them into groups of 8 and do a newline after those 8
                        f.write("\n")

                else:
                    f.write("\n    }")

            if pypos1 != 63: #Last item in list doesn't need a comma
                f.write(",\n")
            else:
                f.write("\n")

        f.write("};\n\n")
        f.close()

def compute_connecting_full_masks():
    """
    Connects two positions on the board from the first to the second, not including the first position.
    It goes from the first position towards the second position until it hits the edge.

    If the two positions cannot be connected, it will return all zeros.
    """

    with open(FILE, "a") as f:
        f.write("const Bitboard connecting_full_masks[64][64] = {\n")

        for pypos1 in range(64):
            pos1 = convert_pos(pypos1)
            x1 = pos1 % 8
            y1 = pos1 // 8
            f.write("    {\n") #Indent

            for pypos2 in range(64):
                pos2 = convert_pos(pypos2)
                x2 = pos2 % 8
                y2 = pos2 // 8
                
                board = np.zeros(64, dtype=int)
                if pos1 == pos2:
                    pass

                elif x1 == x2: #Vertical mask
                    if np.sign(y1 - y2) == 1: #Up
                        for space in range(1, y1 + 1):
                            check_pos = space * -8 + pos1
                            board[check_pos] = 1
                    else: #Down
                        for space in range(1, 8 - y1):
                            check_pos = space * 8 + pos1
                            board[check_pos] = 1

                elif y1 == y2: #Horizontal mask
                    if np.sign(x2 - x1) == 1: #Right
                        for space in range(1, 8 - x1):
                            check_pos = space + pos1
                            board[check_pos] = 1
                    else: #Left
                        for space in range(1, x1):
                            check_pos = -space + pos1
                            board[check_pos] = 1

                elif (y1-y2)/(x2-x1) == 1: #Diagonal, positive slope
                    if (y1 - y2) >= 1: #Up right
                        for s in range(1, min(y1, 7-x1) + 1):
                            board[s * -7 + pos1] = 1

                    else: #Down left
                        for s in range(1, min(7-y1, x1) + 1):
                            board[s * 7 + pos1] = 1


                elif (y1-y2)/(x2-x1) == -1: #Diagonal, negative slope
                    if (y1 - y2) >= 1: #Up left
                        for s in range(1, min(y1, x1) + 1):
                            board[s * -9 + pos1] = 1

                    else: #Down right
                        for s in range(1, min(7-y1, 7-x1) + 1):
                            board[s * 9 + pos1] = 1
                
                else: #Fail safe idk why there really isn't a point cause there's no conceivable way of it ever getting here and the board not being full of zeros but since I wrote it already why not keep it for absolutely no reason but to make myself feel a little better and worse at the same time since it shouldn't break but then also it shouldn't even get to the breaking point in the first place. Well now the only reason why this exists is to host this abusdly long comment that's kinda funny at the time that I'm writing this but might prove to be a little bothersome later.
                    board = np.zeros(64, dtype=int)

                f.write("    ")
                f.write(convert_to_hex_bitboard(board)) 
                if pypos2 != 63: #Last item in list doesn't need a comma
                    f.write(", ")

                    if pypos2 % 8 == 7: #Group them into groups of 8 and do a newline after those 8
                        f.write("\n")

                else:
                    f.write("\n    }")

            if pypos1 != 63: #Last item in list doesn't need a comma
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
        f.write("//A whole ton of masks that are precomputed using masks.py \n")
        f.write("//The Python file just writes all the hex numbers directly into this file as text\n")
        f.write("\n")
        f.write("//It's done this way solely because my Python is better\n")
        f.write("//And also I already had most the code for this implemented and working in Python\n\n")
        f.write("#pragma once \n")
        f.write("#include <stdint.h>\n")
        f.write("typedef uint64_t Bitboard;\n")
        f.write("""
const Bitboard TOP_ROW = 0xFF00000000000000;
const Bitboard BOTTOM_ROW = 0xFF;
const Bitboard RIGHT_COLUMN = 0x8080808080808080;
const Bitboard LEFT_COLUMN = 0x101010101010101;
const Bitboard TOP_TWO = 0xFFFF000000000000;
const Bitboard BOTTOM_TWO = 0xFFFF;
const Bitboard RIGHT_TWO = 0xC0C0C0C0C0C0C0C0;
const Bitboard LEFT_TWO= 0x303030303030303;\n\n"""
        )
        f.close()

    compute_rook_masks()
    compute_bishop_masks()
    compute_knight_masks()
    compute_king_masks()
    compute_all_direction_masks()
    compute_directional_masks()
    compute_connecting_masks()
    compute_connecting_full_masks()
    print("Masks Generated")