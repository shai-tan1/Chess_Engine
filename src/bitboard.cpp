#include <bits/stdc++.h>
#include "bitboard.h"
using namespace std;

U64 bitboards[12];
U64 occupancies[3];
int side = 0;
int enpassant = -1; // -1 means no en passant square available
int castle = 0;

void print_board() {
    cout << "\n";

    // White: P, N, B, R, Q, K | Black: p, n, b, r, q, k
    char pieces[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            int square = row * 8 + col;

            if (col == 0) cout << row + 1 << "  ";

            int piece = -1;
            for (int i = 0; i < 12; i++) {
                if (GET_BIT(bitboards[i], square)) {
                    piece = i;
                    break;
                }
            }

            if (piece == -1) cout << ". ";
            else cout << pieces[piece] << " ";
        }
        cout << "\n";
    }
    cout << "\n   a b c d e f g h\n\n";
    cout << "Side to move: " << (side == white ? "White" : "Black") << "\n";
}

void print_bitboard(U64 bitboard) {
    cout << "\n";
    // 0 is white all the way upto 7 is black
    // start printing from the top row
    for (int row = 7; row >= 0; row--) {
        for (int col = 0; col <= 7; col++) {
            int square = row * 8 + col;
            // each swuare is marked as
            // 56 57 .... 63
            //  .. .
            // 0 1 2 ... 8
            // print the row number
            if (col == 0) cout << row + 1 << "  ";
            // then start printing whether there are any pieces in this square
            if (GET_BIT(bitboard, square)) {
                cout << "1 ";
            }
            else {
                cout << ". ";
            }
        }
        cout << "\n";
    }
    cout << "\n   a b c d e f g h\n\n";
    cout << "Bitboard Value: " << bitboard << "ULL\n";
    cout << "Binary:  " << bitset<64>(bitboard) << "\n";
}

bool is_square_attacked(int square, int side) {
    // 1. Attacked by enemy Pawns? (We use the INVERSE color to look backwards)
    if ((side == white) && (mask_pawn_attacks(black, square) & bitboards[P])) return true;
    if ((side == black) && (mask_pawn_attacks(white, square) & bitboards[p])) return true;

    // Knights
    if (mask_knight_attacks(square) & (side == white ? bitboards[N] : bitboards[n])) return true;

    // Kings
    if (mask_king_attacks(square) & (side == white ? bitboards[K] : bitboards[k])) return true;

    // Bishop / Queens
    if (mask_bishop_attacks(square, occupancies[2]) & (side == white ? (bitboards[B] | bitboards[Q]) : (bitboards[b] | bitboards[q]))) return true;

    // Rooks OR Queens
    if (mask_rook_attacks(square, occupancies[2]) & (side == white ? (bitboards[R] | bitboards[Q]) : (bitboards[r] | bitboards[q]))) return true;

    // If we survive all of that, the square is safe!
    return false;
}

void parse_fen(string fen) {
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));
    side = white;
    enpassant = -1;
    castle = 0;

    int row = 7, col = 0, i = 0;

    while (fen[i] != ' ') {
        char c = fen[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            int piece = -1;
            if (c == 'P') piece = P; else if (c == 'N') piece = N; else if (c == 'B') piece = B;
            else if (c == 'R') piece = R; else if (c == 'Q') piece = Q; else if (c == 'K') piece = K;
            else if (c == 'p') piece = p; else if (c == 'n') piece = n; else if (c == 'b') piece = b;
            else if (c == 'r') piece = r; else if (c == 'q') piece = q; else if (c == 'k') piece = k;

            SET_BIT(bitboards[piece], (row * 8 + col));
            col++;
        }
        else if (c >= '1' && c <= '8') col += (c - '0');
        else if (c == '/') { row--; col = 0; }
        i++;
    }
    i++;
    side = (fen[i] == 'w') ? white : black;
    i += 2;

    while (fen[i] != ' ') {
        if (fen[i] == 'K') castle |= wss;
        else if (fen[i] == 'Q') castle |= wl;
        else if (fen[i] == 'k') castle |= bs;
        else if (fen[i] == 'q') castle |= bl;
        else if (fen[i] == '-') {} // No castling rights
        i++;
    }
    i++;
    if (fen[i] == '-') {
        enpassant = -1;
    }
    else {
        int ep_col = fen[i] - 'a';
        int ep_row = fen[i + 1] - '1';
        enpassant = ep_row * 8 + ep_col;
    }
    for (int j = P; j <= K; j++) occupancies[white] |= bitboards[j];
    for (int j = p; j <= k; j++) occupancies[black] |= bitboards[j];
    occupancies[2] = occupancies[white] | occupancies[black];
}

U64 mask_knight_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    // both the board and attacks are 0 initially
    SET_BIT(bitboard, square);
    // put the knight on the given square

    // from the current index we move in all the possible directions
    attacks |= (bitboard >> 17) & not_h_file;  // Down 2, Left 1--> 2 * 8 + 1
    attacks |= (bitboard >> 15) & not_a_file;  // Down 2, Right 1--> 2 * 8 - 1
    attacks |= (bitboard >> 10) & not_gh_file; // Down 1, Left 2--> 1 * 8 + 2
    attacks |= (bitboard >> 6)  & not_ab_file; // Down 1, Right 2--> 1 * 8 - 2
    attacks |= (bitboard << 17) & not_a_file;  // Up 2, Right 1
    attacks |= (bitboard << 15) & not_h_file;  // Up 2, Left 1
    attacks |= (bitboard << 10) & not_ab_file; // Up 1, Right 2
    attacks |= (bitboard << 6)  & not_gh_file; // Up 1, Left 2
    // since we are alwayss going down the values decrease as the row no decreases
    // thr left cols are closer and right are farther
    // so closer meant we subtract, farther meant add
    // also check if the attacks are valid oe not
    return attacks;
}

U64 mask_king_attacks(int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    // both the board and attacks are 0 initially
    SET_BIT(bitboard, square);

    // from the current index we move in all the possible directions
    attacks |= (bitboard << 1) & not_a_file;  // Right
    attacks |= (bitboard << 9) & not_a_file;  // Up-Right
    attacks |= (bitboard >> 7) & not_a_file;  // Down-Right
    attacks |= (bitboard >> 1) & not_h_file;  // Left
    attacks |= (bitboard << 7) & not_h_file;  // Up-Left
    attacks |= (bitboard >> 9) & not_h_file;  // Down-Left
    attacks |= (bitboard << 8); // Up
    attacks |= (bitboard >> 8); // Down
    // for up down we either go negative or we go after 63 so it is eventually destroyed
    return attacks;
}

U64 mask_pawn_attacks(int side, int square) {
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SET_BIT(bitboard, square);

    if (side == white) {
        // Attack Up-Left (If it wraps, it goes to H. So we block H)
        attacks |= (bitboard << 7) & not_h_file;
        // Attack Up-Right (If it wraps, it goes to A. So we block A)
        attacks |= (bitboard << 9) & not_a_file;
    }
    else {
        // Attack Down-Right (If it wraps, it goes to A. So we block A)
        attacks |= (bitboard >> 7) & not_a_file;
        // Attack Down-Left (If it wraps, it goes to H. So we block H)
        attacks |= (bitboard >> 9) & not_h_file;
    }
    return attacks;
}

U64 mask_rook_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;

    int row = square / 8;
    int col = square % 8;
    // UP
    for (int r = row + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + col));
        if (occupancy & (1ULL << (r * 8 + col))) break;
    }
    // DOWN
    for (int r = row - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + col));
        if (occupancy & (1ULL << (r * 8 + col))) break;
    }
    // RIGHT
    for (int f = col + 1; f <= 7; f++) {
        attacks |= (1ULL << (row * 8 + f));
        if (occupancy & (1ULL << (row * 8 + f))) break;
    }
    // LEFT
    for (int f = col - 1; f >= 0; f--) {
        attacks |= (1ULL << (row * 8 + f));
        if (occupancy & (1ULL << (row * 8 + f))) break;
    }

    return attacks;
}
U64 mask_bishop_attacks(int square, U64 occupancy) {
    U64 attacks = 0ULL;
    int r, f;
    int row = square / 8;
    int col = square % 8;

    // UP-RIGHT
    for (r = row + 1, f = col + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // UP-LEFT
    for (r = row + 1, f = col - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // DOWN-RIGHT
    for (r = row - 1, f = col + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    // DOWN-LEFT
    for (r = row - 1, f = col - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f))) break;
    }
    return attacks;
}

U64 mask_queen_attacks(int square, U64 occupancy) {
    return mask_rook_attacks(square, occupancy) | mask_bishop_attacks(square, occupancy);
}

int parse_square(string sq) {
    int col = sq[0] - 'a';
    int row = sq[1] - '1';
    return row * 8 + col;
}

U64 bitboards_copy[12];
U64 occupancies_copy[3];
int side_copy, enpassant_copy, castle_copy;

bool make_move(int move) {
    int source = GET_MOVE_SOURCE(move);
    int target = GET_MOVE_TARGET(move);
    int piece_to_move = GET_MOVE_PIECE(move);
    int is_ep = GET_MOVE_ENPASSANT(move);
    int is_castle = GET_MOVE_CASTLING(move);
    // 1. SAVE THE STATE
    memcpy(bitboards_copy, bitboards, 96);
    memcpy(occupancies_copy, occupancies, 24);
    side_copy = side; enpassant_copy = enpassant; castle_copy = castle;

    // 2. EN PASSANT CAPTURES (Delete the pawn behind the target)
    if (is_ep) {
        if (side == white) POP_BIT(bitboards[p], target - 8);
        else POP_BIT(bitboards[P], target + 8);
    }

    // 3. NORMAL CAPTURES
    for (int i = 0; i < 12; i++) {
        if (GET_BIT(bitboards[i], target)) {
            POP_BIT(bitboards[i], target);
            break;
        }
    }

    // 4. MOVE THE PIECE
    POP_BIT(bitboards[piece_to_move], source);
    SET_BIT(bitboards[piece_to_move], target);

    int promoted = GET_MOVE_PROMOTED(move);
    if (promoted) {
        // Delete the pawn that just landed on the last rank
        POP_BIT(bitboards[piece_to_move], target);
        // Replace it with the new piece! (e.g., Queen)
        SET_BIT(bitboards[promoted], target);
    }

    // 5. MOVE THE ROOK IF CASTLING
    if (is_castle) {
        if (target == g1) { POP_BIT(bitboards[R], h1); SET_BIT(bitboards[R], f1); } // White Kingside
        else if (target == c1) { POP_BIT(bitboards[R], a1); SET_BIT(bitboards[R], d1); } // White Queenside
        else if (target == g8) { POP_BIT(bitboards[r], h8); SET_BIT(bitboards[r], f8); } // Black Kingside
        else if (target == c8) { POP_BIT(bitboards[r], a8); SET_BIT(bitboards[r], d8); } // Black Queenside
    }

    // 6. REBUILD OCCUPANCIES
    occupancies[white] = 0ULL; occupancies[black] = 0ULL;
    for (int piece = P; piece <= K; piece++) occupancies[white] |= bitboards[piece];
    for (int piece = p; piece <= k; piece++) occupancies[black] |= bitboards[piece];
    occupancies[2] = occupancies[white] | occupancies[black];

    // 7. LEGALITY CHECK (King Safety)
    int king_square = get_lsb_index(side == white ? bitboards[K] : bitboards[k]);
    if (is_square_attacked(king_square, side == white ? black : white)) {
        memcpy(bitboards, bitboards_copy, 96);
        memcpy(occupancies, occupancies_copy, 24);
        side = side_copy; enpassant = enpassant_copy; castle = castle_copy;
        return false;
    }

    // 8. UPDATE STATE FOR NEXT TURN
    if (piece_to_move == P && target == source + 16) enpassant = source + 8;
    else if (piece_to_move == p && target == source - 16) enpassant = source - 8;
    else enpassant = -1;

    // 9. UPDATE CASTLING RIGHTS
    // If a King moves, strip both of its castling rights.
    if (piece_to_move == K) castle &= ~(wss | wl);
    if (piece_to_move == k) castle &= ~(bs | bl);

    // If a Rook moves (or is captured!), strip that specific side's castling right.
    if (source == h1 || target == h1) castle &= ~wss;
    if (source == a1 || target == a1) castle &= ~wl;
    if (source == h8 || target == h8) castle &= ~bs;
    if (source == a8 || target == a8) castle &= ~bl;
    // (Note: To be fully complete, enpassant and castle rights need to update here based on the move,
    // but this is enough to test the physics!)
    // 10. CHANGE TURNS
    side = (side == white) ? black : white;
    return true;
}

int count_bits(U64 bitboard) {
    int count = 0;
    while (bitboard > 0) {
        count++;
        bitboard &= (bitboard - 1);
    }
    return count;
}

int get_lsb_index(U64 bitboard) {
    if (bitboard == 0) return -1;
    return __builtin_ctzll(bitboard);
}

static inline void add_move(MoveList *move_list, int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}

void generate_moves(MoveList *move_list) {
    move_list->count = 0;

    // --- 1. PAWN MOVES ---
    int pawn = (side == white) ? P : p;
    U64 pawn_bitboard = bitboards[pawn];

    while (pawn_bitboard > 0) {
        int source = get_lsb_index(pawn_bitboard);

        if (side == white) {
            // A. Single Push
            int target = source + 8;
            if (target <= h8 && !GET_BIT(occupancies[2], target)) {
                // Promotion Check
                if (source >= a7 && source <= h7) {
                    add_move(move_list, ENCODE_MOVE(source, target, P, Q, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, P, R, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, P, B, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, P, N, 0, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, target, P, 0, 0, 0, 0, 0));
                    // B. Double Push
                    if (source >= a2 && source <= h2) {
                        int double_target = source + 16;
                        if (!GET_BIT(occupancies[2], double_target)) {
                            add_move(move_list, ENCODE_MOVE(source, double_target, P, 0, 0, 1, 0, 0));
                        }
                    }
                }
            }
            // C. Pawn Captures
            U64 attacks = mask_pawn_attacks(white, source) & occupancies[black];
            while (attacks > 0) {
                int cap_target = get_lsb_index(attacks);
                // Promotion Capture Check
                if (source >= a7 && source <= h7) {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, Q, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, R, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, B, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, N, 1, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, P, 0, 1, 0, 0, 0));
                }
                POP_BIT(attacks, cap_target);
            }
            // D. En Passant
            if (enpassant != -1 && source >= a5 && source <= h5) {
                U64 ep_attacks = mask_pawn_attacks(white, source) & (1ULL << enpassant);
                if (ep_attacks > 0) {
                    add_move(move_list, ENCODE_MOVE(source, enpassant, P, 0, 1, 0, 1, 0));
                }
            }
        }
        else { // BLACK PAWN LOGIC
            // A. Single Push
            int target = source - 8;
            if (target >= a1 && !GET_BIT(occupancies[2], target)) {
                if (source >= a2 && source <= h2) {
                    add_move(move_list, ENCODE_MOVE(source, target, p, q, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, p, r, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, p, b, 0, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, target, p, n, 0, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, target, p, 0, 0, 0, 0, 0));
                    // B. Double Push
                    if (source >= a7 && source <= h7) {
                        int double_target = source - 16;
                        if (!GET_BIT(occupancies[2], double_target)) {
                            add_move(move_list, ENCODE_MOVE(source, double_target, p, 0, 0, 1, 0, 0));
                        }
                    }
                }
            }
            // C. Pawn Captures
            U64 attacks = mask_pawn_attacks(black, source) & occupancies[white];
            while (attacks > 0) {
                int cap_target = get_lsb_index(attacks);
                if (source >= a2 && source <= h2) {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, q, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, r, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, b, 1, 0, 0, 0));
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, n, 1, 0, 0, 0));
                } else {
                    add_move(move_list, ENCODE_MOVE(source, cap_target, p, 0, 1, 0, 0, 0));
                }
                POP_BIT(attacks, cap_target);
            }
            // D. En Passant
            if (enpassant != -1 && source >= a4 && source <= h4) {
                U64 ep_attacks = mask_pawn_attacks(black, source) & (1ULL << enpassant);
                if (ep_attacks > 0) {
                    add_move(move_list, ENCODE_MOVE(source, enpassant, p, 0, 1, 0, 1, 0));
                }
            }
        }
        POP_BIT(pawn_bitboard, source);
    }

    // --- 2. KNIGHT MOVES ---
    int knight = (side == white) ? N : n;
    U64 knight_bitboard = bitboards[knight];

    while (knight_bitboard > 0) {
        int source = get_lsb_index(knight_bitboard);
        U64 attacks = mask_knight_attacks(source) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            int target = get_lsb_index(attacks);
            int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, knight, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(knight_bitboard, source);
    }

    // --- 3. KING MOVES ---
    int king = (side == white) ? K : k;
    U64 king_bitboard = bitboards[king];

    while (king_bitboard > 0) {
        int source = get_lsb_index(king_bitboard);
        U64 attacks = mask_king_attacks(source) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            int target = get_lsb_index(attacks);
            int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, king, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(king_bitboard, source);
    }

    // --- 4. BISHOP MOVES ---
    int bishop = (side == white) ? B : b;
    U64 bishop_bitboard = bitboards[bishop];

    while (bishop_bitboard > 0) {
        int source = get_lsb_index(bishop_bitboard);
        // Note: Sliding pieces need the global occupancy (occupancies[2]) to know where to stop sliding!
        U64 attacks = mask_bishop_attacks(source, occupancies[2]) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            int target = get_lsb_index(attacks);
            int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, bishop, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(bishop_bitboard, source);
    }

    // --- 5. ROOK MOVES ---
    int rook = (side == white) ? R : r;
    U64 rook_bitboard = bitboards[rook];

    while (rook_bitboard > 0) {
        int source = get_lsb_index(rook_bitboard);
        U64 attacks = mask_rook_attacks(source, occupancies[2]) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            int target = get_lsb_index(attacks);
            int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, rook, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(rook_bitboard, source);
    }

    // --- 6. QUEEN MOVES ---
    int queen = (side == white) ? Q : q;
    U64 queen_bitboard = bitboards[queen];

    while (queen_bitboard > 0) {
        int source = get_lsb_index(queen_bitboard);
        U64 attacks = mask_queen_attacks(source, occupancies[2]) & ~(side == white ? occupancies[white] : occupancies[black]);

        while (attacks > 0) {
            int target = get_lsb_index(attacks);
            int capture = GET_BIT((side == white ? occupancies[black] : occupancies[white]), target) ? 1 : 0;
            add_move(move_list, ENCODE_MOVE(source, target, queen, 0, capture, 0, 0, 0));
            POP_BIT(attacks, target);
        }
        POP_BIT(queen_bitboard, source);
    }

    // --- 7. CASTLING MOVES ---
    if (side == white) {
        // Kingside Castling (Short)
        if (castle & wss) {
            // Check if f1 (5) and g1 (6) are empty
            if (!GET_BIT(occupancies[2], f1) && !GET_BIT(occupancies[2], g1)) {
                // The King cannot be in check, and cannot walk through check
                if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black) && !is_square_attacked(g1, black)) {
                    // Suitcase: source, target, piece, promoted, capture=0, double=0, ep=0, castle=1
                    add_move(move_list, ENCODE_MOVE(e1, g1, K, 0, 0, 0, 0, 1));
                }
            }
        }
        // Queenside Castling (Long)
        if (castle & wl) {
            // Check if d1 (3), c1 (2), b1 (1) are empty
            if (!GET_BIT(occupancies[2], d1) && !GET_BIT(occupancies[2], c1) && !GET_BIT(occupancies[2], b1)) {
                if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black) && !is_square_attacked(c1, black)) {
                    add_move(move_list, ENCODE_MOVE(e1, c1, K, 0, 0, 0, 0, 1));
                }
            }
        }
    } else {
        // Kingside Castling (Black Short)
        if (castle & bs) {
            // Check if f8 (61) and g8 (62) are empty
            if (!GET_BIT(occupancies[2], f8) && !GET_BIT(occupancies[2], g8)) {
                if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white) && !is_square_attacked(g8, white)) {
                    add_move(move_list, ENCODE_MOVE(e8, g8, k, 0, 0, 0, 0, 1));
                }
            }
        }
        // Queenside Castling (Black Long)
        if (castle & bl) {
            // Check if d8 (59), c8 (58), b8 (57) are empty
            if (!GET_BIT(occupancies[2], d8) && !GET_BIT(occupancies[2], c8) && !GET_BIT(occupancies[2], b8)) {
                if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white) && !is_square_attacked(c8, white)) {
                    add_move(move_list, ENCODE_MOVE(e8, c8, k, 0, 0, 0, 0, 1));
                }
            }
        }
    }
}