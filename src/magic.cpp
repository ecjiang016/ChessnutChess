#include "magic.h"
#include <array>
#include <random>
#include <iomanip>

const Bitboard Magic::bishop_magics[64] = {
    0x01a0901400450000,
    0x1400020806040020,
    0x0300020224000810,
    0x2402040014002010,
    0x4800000846058000,
    0x0842010020080200,
    0x0000011240000002,
    0x4088000244400000,
    0x000000c000831913,
    0x0088404402000140,
    0x0001641408042080,
    0x00830400c2018100,
    0x0002001092000000,
    0x8000401129000400,
    0x1000020001008022,
    0x0002600004008000,
    0x0540a44001001008,
    0x0000000000102014,
    0xa2000080000020c2,
    0xc00018000022c000,
    0x2008010000244104,
    0x4000210500400001,
    0x000d12010b100100,
    0x100e00300021102c,
    0x00203c1282040001,
    0x0010004182100020,
    0x0040140200980800,
    0x15c6040000100000,
    0x000000008a000000,
    0x2090800141150200,
    0x240a400080010000,
    0xc40101000601c404,
    0x0200005200100180,
    0x0088000004218100,
    0x0400020009204500,
    0x0220211800000001,
    0x0000320020000220,
    0x0900000a0c144400,
    0x0080500801014010,
    0x0000010008000120,
    0xa800000000100400,
    0x4084044800004000,
    0x00c0040a00010050,
    0x04400000a1903000,
    0x800c204002000200,
    0x0004004202042000,
    0x0800400001040024,
    0x0008220200800003,
    0x000004480810c180,
    0x1003080200b00800,
    0x0002340900008000,
    0x80251140b0141001,
    0x0020800100482102,
    0x81c02000090021e0,
    0x00440020a1080000,
    0x000000010e400102,
    0x1028200000000500,
    0x4101094018018000,
    0x0a00000000015000,
    0x0003041000200400,
    0x0205010215004002,
    0x0108808800241019,
    0x2000102000104850,
    0x5040000020400000
};

const Bitboard Magic::rook_magics[64] = {
    0x04a0010260000000,
    0x062004800000030d,
    0x4002000000060021,
    0x0202020401000010,
    0x8000080182010009,
    0x0001101a00021484,
    0x208a900000200000,
    0x2000090084c00002,
    0x44040a3840100000,
    0x0200008000000100,
    0x0400012020000021,
    0x0090000000000418,
    0x0400000b44090020,
    0x1040200001008040,
    0x0400002240000080,
    0x0000000080140042,
    0x0205000080001008,
    0x0400280500002008,
    0x0003000040084000,
    0x0001308064020002,
    0x1100001002400226,
    0x0800000400000002,
    0x0400402808640082,
    0x0011040001008404,
    0x4000860004240095,
    0x00900a0008200900,
    0x20000000801c2c08,
    0x2003000002800003,
    0x1000002400050823,
    0x008000a070000180,
    0x0040008001882880,
    0x0000200100000400,
    0xc400000121840008,
    0x0090440008644048,
    0x40008a24a2000640,
    0x000410000b08002c,
    0x0a00006000048012,
    0x2440000080000221,
    0x0031100020000000,
    0x0200000286002000,
    0x0020206102100420,
    0x2804c14080086204,
    0x0090002080001004,
    0x400054000a002042,
    0x020002012040002c,
    0x0000000080080002,
    0x000400c200000008,
    0x0400280000409000,
    0x2088412911002080,
    0x00001c0ac0000004,
    0x0000208000000000,
    0x080602081804021b,
    0x4040410600101208,
    0x0002644001202000,
    0x020008a008002048,
    0xc201408000029800,
    0x0120003020100001,
    0xa040000000468200,
    0x1000200401440310,
    0x0180802808031201,
    0x0800106410400402,
    0x0004800000880204,
    0x40000801a0100090,
    0x0040104002800803
};

//Attacks computed with hyperbola quintessence
//Pretty much used to pre-compute moves for the faster magic bitboard move gen
// sliding_moves is taken from moves.h and also put here to fix linking issues

inline Bitboard sliding_moves(Bitboard occupancy, Bitboard mask, Bitboard piece_square_bitboard) {
    return (((occupancy & mask) - piece_square_bitboard) ^
        bswap_64(bswap_64(occupancy & mask) - bswap_64(piece_square_bitboard))) & mask;
}

template<PieceType = Bishop>
inline Bitboard HQ_attacks(int pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, bishop_masks_diag1[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, bishop_masks_diag2[pos_idx], get_single_bitboard(pos_idx));
}

template<>
inline Bitboard HQ_attacks<Rook>(int pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, rook_masks_horizontal[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, rook_masks_vertical[pos_idx],   get_single_bitboard(pos_idx));
}

namespace Magic {

    Bitboard rook_attacks[64][512];
    Bitboard bishop_attacks[64][4096];

    Bitboard getOccupancy(int perm_idx, Bitboard mask) { //Generates a permutation of the occupancy of mask
        Bitboard out = 0;
        int i = 0;
        while (mask) {
            Bitboard ls1b = mask & -mask;
            mask ^= ls1b; 
            if (check_bit(perm_idx, i))
                out |= ls1b;
            i++;
        }
        return out;
    }

    template<PieceType piece_type>
    void initializeSingleTable() {
        for (int sq = 0; sq < 64; sq++) {
            Bitboard mask = (piece_type == Bishop) ? bishop_masks[sq] : rook_masks[sq];
            Bitboard magic = (piece_type == Bishop) ? bishop_magics[sq] : rook_magics[sq];
            uint8_t mask_pop = (piece_type == Bishop) ? bishop_shifts[sq] : rook_shifts[sq];
            for (int i = 0; i < (1 << mask_pop); i++) {
                Bitboard occupancy = getOccupancy(i, mask);
                Bitboard index = (occupancy * magic) >> mask_pop;
                Bitboard attacks = HQ_attacks<piece_type>(sq, occupancy);
                if constexpr (piece_type == Bishop) {
                    bishop_attacks[sq][index] = attacks;
                } else {
                    rook_attacks[sq][index] = attacks;
                }
            }
        }
    }

    void initializeTables() {
        std::cout << "Bishop" << std::endl;
        initializeSingleTable<Bishop>();
        std::cout << "Rook" << std::endl;
        initializeSingleTable<Rook>();
    }

    template<PieceType piece_type>
    void findMagics() {
        //Some stuff for generate random magics
        std::random_device rd;
        std::mt19937_64 rng(rd());
        std::uniform_int_distribution<unsigned long long> dist(Bitboard(1), Bitboard(-1));
        auto random = [&]() { return dist(rng); };

        Bitboard magics[64];
        for (int i = 0; i < 64; i++) magics[i] = 0;

        constexpr size_t CACHE_SIZE = (piece_type == Bishop) ? (1 << 9) : (1 << 12); 
        Bitboard index_cache[CACHE_SIZE]; //Stores already occupied indices and the corresponding attack bitboard.
                                          //The biggest possible index is 2^9 for bishops and 2^12 for rooks
        while (magics[63] == 0) {
            for (int square = 0; square < CACHE_SIZE; square++) {
                Bitboard magic = random() & random() & random();
                Bitboard mask = (piece_type == Bishop) ? bishop_masks[square] : rook_masks[square];
                for (int i = 0; i < CACHE_SIZE; i++) index_cache[i] = Bitboard(-1); //Clear cache by setting all bits. An attack can be empty but cannot be all spaces.
                int mask_pop = pop_count(mask);
                // Loop through all 2^n possible permutations of the mask where n is the number of bits set in the mask
                bool working_magic = true;
                for (int i = 0; i < (1 << mask_pop); i++) {
                    Bitboard occupancy = getOccupancy(i, mask);
                    Bitboard index = (occupancy * magic) >> (64 - mask_pop);
                    Bitboard attacks = HQ_attacks<piece_type>(square, occupancy);
                    if (index_cache[index] != Bitboard(-1) && index_cache[index] != attacks) { //There is a collision and the contents are different
                        working_magic = false;
                        break;
                    }
                }

                if (!working_magic)
                    break;

                magics[square] = magic;

            }
        }

        for (int i = 0; i < 64; i++) {
            std::cout << "    " << "0x" << std::setfill ('0') << std::setw(16) << std::hex << magics[i];
            if (i != 63)
                std::cout << ',';
            std::cout << '\n';
        }
        std::cout << "};\n\n";

    }

    //Wrapper for findMagics()
    void computeMagics() {
        std::cout << "const Bitboard Magic::bishop_magics[64] = {\n";
        findMagics<Bishop>();
        std::cout << "const Bitboard Magic::rook_magics[64] = {\n";
        findMagics<Rook>();
    }

}