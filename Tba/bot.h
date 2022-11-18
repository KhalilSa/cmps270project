#ifndef BOT_H
#define BOT_H

#include "bool.h"

// Structures

typedef struct {
    int column, score;
} BestMove;

// Bot functions

int make_move(int **board);
BestMove minimax(int **board, int depth, int alpha, int beta, Boolean is_maximazing_player);
int* get_valid_locations(int **board);
int score_board(int** board, int token);
int random_valid_column(int* valid_columns);
int score_bucket(int* bucket, int token, int start, int end);

#endif