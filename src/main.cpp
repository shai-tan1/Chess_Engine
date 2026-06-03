#include <bits/stdc++.h>
using namespace std;
#include "types.h"
#include "bitboard.h"

int count_bits(U64 bitboard);
int get_lsb_index(U64 bitboard);

int main() {

    cout << "--- KNIGHT Attacks from d4 ---\n";
    U64 knight_map = mask_knight_attacks(d4);
    print_bitboard(knight_map);

    cout << "--- KING Attacks from e4 ---\n";
    U64 king_map = mask_king_attacks(e4);
    print_bitboard(king_map);

    cout << "--- WHITE PAWN Attacks from e2 ---\n";
    U64 white_pawn_map = mask_pawn_attacks(white, e2);
    print_bitboard(white_pawn_map);

    cout << "--- BLACK PAWN Attacks from d7 ---\n";
    U64 black_pawn_map = mask_pawn_attacks(black, d7);
    print_bitboard(black_pawn_map);

    U64 occupancy = 0ULL;

    SET_BIT(occupancy, d6); // Block Up
    SET_BIT(occupancy, d2); // Block Down
    SET_BIT(occupancy, b4); // Block Left
    SET_BIT(occupancy, g4); // Block Right
    SET_BIT(occupancy, b6); // Block Up-Left
    SET_BIT(occupancy, f6); // Block Up-Right
    SET_BIT(occupancy, b2); // Block Down-Left
    SET_BIT(occupancy, f2); // Block Down-Right

    print_bitboard(occupancy);
    U64 rook_map = mask_rook_attacks(d4, occupancy);
    print_bitboard(rook_map);

    U64 bishop_map = mask_bishop_attacks(d4, occupancy);
    print_bitboard(bishop_map);

    U64 queen_map = mask_queen_attacks(d4, occupancy);
    print_bitboard(queen_map);
}

int count_bits(U64 bitboard) {
    int count = 0;
    while (bitboard > 0) {
        count++;
        bitboard &= (bitboard - 1);
    }
    /// we go from 111101000 ---> 111100000
    /// last set bit is removed
    /// most optimally counts set bits
    return count;
}

int get_lsb_index(U64 bitboard) {
    // 0 will break ctzll
    if (bitboard == 0) return -1;
    // number of 0s before the first 1
    return __builtin_ctzll(bitboard);
}