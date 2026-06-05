#pragma once
#include <string>

using namespace std;

void uci_loop();
int parse_move(const string& move_string);
void print_move(int move);