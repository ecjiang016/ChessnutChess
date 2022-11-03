#include "magic.h"
#include <array>
#include <random>
#include <iomanip>

typedef uint8_t Square;

const Bitboard Magic::bishop_magics[64] = {
    0x2210200204002020,
    0x2008100080810900,
    0x4010041080200400,
    0x0088084900882048,
    0x0010882080028400,
    0x0002020220000005,
    0x010301084a400000,
    0xc080b04410241000,
    0x0120401012008104,
    0x8000040882040020,
    0x0000100898810008,
    0x0104040407800880,
    0x001002021000200a,
    0x0408008221200006,
    0xc402820110121000,
    0x0002042c44040508,
    0x1808009010290800,
    0x264805a101043084,
    0x1010002841920010,
    0x0808000088630000,
    0x0104000080a00248,
    0x0043000201008200,
    0x800b000421080220,
    0x80e1411100482400,
    0x002240a81a080808,
    0x420804ae08010800,
    0x0008021014002208,
    0x0104080004012002,
    0x0080820014010402,
    0x00500040a0880802,
    0x4002020004880180,
    0x000a042002008208,
    0x8004442000428201,
    0x0302017040041022,
    0x000410c400082800,
    0x00041401082c0100,
    0x1030020080263004,
    0x0030050a00024041,
    0x0046149600040a00,
    0x00520c0500802084,
    0x50210d282104c000,
    0x0058420804002000,
    0x2000108403005000,
    0x5440020322000c00,
    0x80b2040104010210,
    0x0901311000802100,
    0x4010960085000428,
    0x8210048520480100,
    0x0100411010120020,
    0x0011410c10020082,
    0x4490220042080241,
    0x8000080205040082,
    0x1000001102020004,
    0x0000400848036000,
    0x0084100488028000,
    0x088c0d8202020202,
    0x00010308020a0200,
    0x2680002101082000,
    0x8000020040443020,
    0x8082802000420220,
    0x0204004220a24408,
    0x1a82904008019101,
    0x0400400521021200,
    0x4008080100c40100
};

const Bitboard Magic::rook_magics[64] = {
    0x0080001040038022,
    0x0140002000401000,
    0x0200084202802050,
    0x0100141000612900,
    0x420002000c200810,
    0x0a00084200045041,
    0x0080020011000280,
    0x8200090400804066,
    0x0c00800040002080,
    0x38004001200b5000,
    0x0000802000100180,
    0x0481000900221000,
    0x0008800400080080,
    0x0448800400800600,
    0x008c000417421008,
    0x4010801064800100,
    0x1080008020804001,
    0x002092802004c000,
    0x2011220030804200,
    0x108121002b003000,
    0x23ea808004001800,
    0x0224004022004100,
    0x4014040090410a08,
    0x008002002288c104,
    0x0000400080048020,
    0x1200200040100248,
    0x00a2832200104200,
    0x0020420200119820,
    0x2009009500500800,
    0x0103000900061400,
    0x0000080400302201,
    0x900100030000a242,
    0x0000400024800088,
    0x001000a000c000c1,
    0x0088200282801000,
    0x0080100080800802,
    0x0182b09801000d00,
    0x0482002892001004,
    0x1b00800100800200,
    0xa041000085000542,
    0x0001400084208000,
    0x803004402008c000,
    0x0410028060008010,
    0x0028020100501000,
    0x10080008d1010024,
    0x0026600440980110,
    0x4002431008040002,
    0x0012a0cc00820031,
    0x1000c080010a2300,
    0x0d01004010822100,
    0xac02200102124100,
    0x0008100082480080,
    0x0010040082080080,
    0x0002000400800280,
    0x2001010882100400,
    0x0800803043000080,
    0x0006002142810012,
    0x0140009044648501,
    0x41a0084090208202,
    0x0210500100042009,
    0x0842002008114432,
    0x240200881430111e,
    0x4011b00188260104,
    0x0001000080204201
};

//Attacks computed with hyperbola quintessence
//Pretty much used to pre-compute moves for the faster magic bitboard move gen
// sliding_moves is taken from moves.h and also put here to fix linking issues

inline Bitboard sliding_moves(Bitboard occupancy, Bitboard mask, Bitboard piece_square_bitboard) {
    return (((occupancy & mask) - piece_square_bitboard) ^
        bswap_64(bswap_64(occupancy & mask) - bswap_64(piece_square_bitboard))) & mask;
}

template<PieceType = Bishop>
inline Bitboard HQ_attacks(Square pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, bishop_masks_diag1[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, bishop_masks_diag2[pos_idx], get_single_bitboard(pos_idx));
}

template<>
inline Bitboard HQ_attacks<Rook>(Square pos_idx, Bitboard occupancy) {
    return sliding_moves(occupancy, rook_masks_horizontal[pos_idx], get_single_bitboard(pos_idx)) | 
           sliding_moves(occupancy, rook_masks_vertical[pos_idx],   get_single_bitboard(pos_idx));
}

namespace Magic {

    Bitboard bishop_attacks[64][512];
    Bitboard rook_attacks[64][4096];

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
        for (Square sq = 0; sq <= 63; sq++) {
            Bitboard mask = (piece_type == Bishop) ? bishop_masks[sq] : rook_masks[sq];
            Bitboard magic = (piece_type == Bishop) ? bishop_magics[sq] : rook_magics[sq];
            uint8_t mask_pop = pop_count(mask);
            for (int i = 0; i < (1 << mask_pop); i++) {
                Bitboard occupancy = getOccupancy(i, mask);
                Bitboard index = (occupancy * magic) >> ((piece_type == Bishop) ? bishop_shifts[sq] : rook_shifts[sq]);
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
        initializeSingleTable<Bishop>();
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
        Bitboard magic;
        for (int i = 0; i < 64; i++) magics[i] = 0;

        constexpr size_t CACHE_SIZE = (piece_type == Bishop) ? (1 << 9) : (1 << 12); 
        Bitboard index_cache[CACHE_SIZE]; //Stores already occupied indices and the corresponding attack bitboard.
                                          //The biggest possible index is 2^9 for bishops and 2^12 for rooks
        while (magics[63] == 0) {
            for (Square square = 0; square <= 63; square++) {
                while (true) {
                    magic = random() & random() & random();
                    Bitboard mask = (piece_type == Bishop) ? bishop_masks[square] : rook_masks[square];
                    for (int i = 0; i < CACHE_SIZE; i++) index_cache[i] = Bitboard(-1); //Clear cache by setting all bits. An attack can be empty but cannot be all spaces.
                    int mask_pop = pop_count(mask);
                    bool working_magic = true;
                    // Loop through all 2^n possible permutations of the mask where n is the number of bits set in the mask
                    for (int i = 0; i < (1 << mask_pop); i++) {
                        Bitboard occupancy = getOccupancy(i, mask);
                        Bitboard index = (occupancy * magic) >> (64 - mask_pop);
                        Bitboard attacks = HQ_attacks<piece_type>(square, occupancy);
                        if (index_cache[index] != Bitboard(-1) && index_cache[index] != attacks) { //There is a collision and the contents are different
                            working_magic = false;
                            break;
                        }
                        index_cache[index] = attacks;
                    }

                    if (working_magic)
                        break;

                }
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