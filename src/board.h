#pragma once
#include "types.h"
#include <string>
using namespace std;

void parse_fen(const string &fen);
void print_board();
void print_bitboard(const U64 bitboard);
int parse_square(const string &sq);