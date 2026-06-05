#pragma once
#include "types.h"

// --- MOVE ENCODING ---
// Pack a move into a single integer
#define ENCODE_MOVE(source, target, piece, promoted, capture, double_push, enpassant, castling) \
((source) | \
(target << 6) | \
(piece << 12) | \
(promoted << 16) | \
(capture << 20) | \
(double_push << 21) | \
(enpassant << 22) | \
(castling << 23))

// Extract information back out of the integer
#define GET_MOVE_SOURCE(move)      (move & 0x3f)          // 0x3f is 63 in hex (6 bits)
#define GET_MOVE_TARGET(move)      ((move >> 6) & 0x3f)
#define GET_MOVE_PIECE(move)       ((move >> 12) & 0xf)   // 0xf is 15 in hex (4 bits)
#define GET_MOVE_PROMOTED(move)    ((move >> 16) & 0xf)
#define GET_MOVE_CAPTURE(move)     ((move >> 20) & 0x1)
#define GET_MOVE_DOUBLE(move)      ((move >> 21) & 0x1)
#define GET_MOVE_ENPASSANT(move)   ((move >> 22) & 0x1)
#define GET_MOVE_CASTLING(move)    ((move >> 23) & 0x1)

struct MoveList {
    int moves[256];
    int scores[256];
    int count;      // How many moves are currently in the list
};

static inline void add_move(MoveList *move_list, const int move) {
    move_list->moves[move_list->count] = move;
    move_list->count++;
}