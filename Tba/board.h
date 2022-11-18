#ifndef BOARD_H
#define BOARD_H

#include "bool.h"

// Main functions

void init_board(int** board);
void choose(int** board, int mode);
void fill_column(int **board, int column, int player);
Boolean check_winner(int **board, int token);
double choose_with_timer(int **board, int mode, void (*choose)(int** board, int mode));

#endif