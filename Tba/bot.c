#include <stdlib.h>

#include "globals.h"
#include "board.h"
#include "utils.h"
#include "bot.h"


#define DEBUG_CENTER_SCORING 1
#define DEBUG_HORIZONTAL_SCORING 1
#define DEBUG_VERTICAL_SCORING 1
#define DEBUG_POSDIAG_SCORING 1
#define DEBUG_NEGDIAG_SCORING 1

/*
    Makes an intellegent move
*/
int make_move(int **board) {
    #if DEBUG
        Boolean *valid_locations = get_valid_locations(board);
        int best_score = 0;
        int best_col = random_valid_column(valid_locations);
        for (int col = 0; col < COLS; col++) {
            if (!valid_locations[col]) continue;
            int** board_copy = (int**) alloca(ROWS * sizeof(int *));
            for (int i = 0; i < ROWS; i++) {
                board_copy[i] = (int *) alloca(COLS * sizeof(int));
            }
            copy_board(board, board_copy);
            fill_column(board_copy, col, g_ai_piece);
            int score = score_board(board_copy, g_ai_piece);
            if (score > best_score) {
                best_score = score;
                best_col = col;
            }
        }
        return best_col;
    #else
        // the optimal move for the first move is always in the middle
        // I'm hard coding (caching) the move to save computation
        if (g_moves < 3) return COLS / 2;
        return minimax(board, g_difficulty, -__INT_MAX__, __INT_MAX__, True).column;
    #endif
}

/*
    returns the score and the column of the best (not always optimal) possible move
*/
BestMove minimax(int **board, int depth, int alpha, int beta, Boolean is_maximazing_player) {
    Boolean *valid_locations = get_valid_locations(board);
    Boolean ai_won = check_winner(board, g_ai_piece);
    Boolean player_won = check_winner(board, g_player_piece);
    Boolean board_full = g_moves >= ROWS * COLS ? True : False;
    
    Boolean is_game_over = ai_won || player_won || board_full;

    int score, column;

    if (depth == 0 || is_game_over) {
        if (is_game_over) { 
            if (ai_won) {
                return (BestMove){-1, 100000};
            }
            else if (player_won) {
                return (BestMove){-1, -100000};
            } 
            // draw
            else {
                return (BestMove){-1, 0};
            }
        }
        // depth = 0
        else {
            score = score_board(board, g_ai_piece);
            return (BestMove){-1, score};
        }
    }

    if (is_maximazing_player) {
        score = -__INT32_MAX__;
        column = random_valid_column(valid_locations);
        for (int col = 0; col < COLS; col++) {
            if (!valid_locations[col]) continue;
            int** board_copy = (int**) alloca(ROWS * sizeof(int *));
            for (int i = 0; i < ROWS; i++) {
                board_copy[i] = (int *) alloca(COLS * sizeof(int));
            }
            copy_board(board, board_copy);
            fill_column(board_copy, col, g_ai_piece);
            int new_score = minimax(board_copy, depth - 1, alpha, beta, False).score;
            if (new_score > score) {
                score = new_score;
                column = col;
            }
            alpha = MAX(alpha, score);
            if (alpha >= beta) break;
        }
    } else {
        score = __INT32_MAX__;
        column = random_valid_column(valid_locations);
        for (int col = 0; col < COLS; col++) {
            if (!valid_locations[col]) continue;
            int** board_copy = (int**) alloca(ROWS * sizeof(int *));
            for (int i = 0; i < ROWS; i++) {
                board_copy[i] = (int *) alloca(COLS * sizeof(int));
            }
            copy_board(board, board_copy);
            fill_column(board_copy, col, g_player_piece);
            int new_score = minimax(board_copy, depth - 1, alpha, beta, True).score;
            if (new_score < score) {
                score = new_score;
                column = col;
            }
            beta = MIN(beta, score);
            if (alpha >= beta) break;
        }
    }
    return (BestMove) {column, score};
}

/* returns an array of booleans in which the value of the array at the index representing the column represents the availability of the column
1 if full, 0 if it still has capacity */
int* get_valid_locations(int **board) {
    static Boolean valid_columns[COLS];
    for (int i = 0; i < COLS; i++) {
        // if the column still not full mark it as valid (1) location, otherwise mark it as invalid (0)
        if (!board[0][i]) valid_columns[i] = True;
        else valid_columns[i] = 0;
    }
    return valid_columns;
}

int random_valid_column(Boolean* valid_columns) {
    int random_col = generate_random_int(0, COLS);

    // if not return the closest valid column
    int i = 0;
    while (i < COLS || !valid_columns[random_col]) {
        random_col++;
        random_col %= COLS;
        i++;
    }
    return random_col;
}

/* Gives a heuristic score for the current board */
int score_board(int** board, int token) {
    int score = 0;

    #if DEBUG_CENTER_SCORING
    // Score Center Column (prefers center at the beginning of the game)
    if (g_moves <= 6) {
        int center_column[ROWS];
        int mid = COLS / 2;
        for (int i = 0; i < ROWS; i++) {
            center_column[i] = board[i][mid];
        }
        int center_tokens_num = count_token(center_column, token, 0, ROWS);
        score += center_tokens_num * 5;
        #endif
    }

    #if DEBUG_HORIZONTAL_SCORING
    // Score Horizontal
    for (int i = 0; i < ROWS; i++) {
        // row of 7 elements
        int* row_array = board[i];
        for (int j = 0; j < COLS - 3; j++) {
            int start = j;
            int end = j + 4;
            if (i < ROWS - 1) {
                for (int k = start; k < end; k++) {
                    if (board[i+1][k] == EMPTY) continue;
                }
            }
            int new_score = score_bucket(row_array, token, start, end);
            score += new_score;
        }
    }
    #endif

    #if DEBUG_VERTICAL_SCORING
    // Score Vertical
    for (int j = 0; j < COLS; j++) {
        // column of 6 elements
        int col_array[ROWS];

        for (int i = 0; i < ROWS; i++) {
            col_array[i] = board[i][j];
        }
        for (int i = 0; i < ROWS - 3; i++) {
            int start = i;
            int end = i + 4;
            int new_score = score_bucket(col_array, token, start, end);
            score += new_score;
        }
    }
    #endif

    #if DEBUG_POSDIAG_SCORING
    // score Ascending diagonals
    // ascending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=0; j < COLS-3; j++) {
            int diagonal_bucket[4];
            for (int k = 0; k < 4; k++) {
                diagonal_bucket[k] = board[i-k][j+k];
                if (i != ROWS - 1 || k != 0) {
                    if (board[i-k+1][j+k]) continue;
                }
            }
            int new_score = score_bucket(diagonal_bucket, token, 0, 4);
            score += new_score;
            
        }
    }
    #endif

    #if DEBUG_NEGDIAG_SCORING
    // descending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=3; j < COLS; j++) {
            int diagonal_bucket[4];
            for (int k = 0; k < 4; k++) {
                diagonal_bucket[k] = board[i-k][j-k];
                if (i != ROWS - 1 || k != 0) {
                    if (board[i-k+1][j-k]) continue;
                }
            }
            int new_score = score_bucket(diagonal_bucket, token, 0, 4);
            score += new_score;
        }
    }
    #endif
    return score;
}

/* Gives a heuristic score the current bucket (window) of four tokens */
int score_bucket(int* bucket, int token, int start, int end) {
    int opp_piece = g_player_piece;
    if (token == g_player_piece) {
        opp_piece = g_ai_piece;
    }

    int score = 0;

    int token_num = count_token(bucket, token, start, end);
    int opp_token_num = count_token(bucket, opp_piece, start, end);
    #if OLD_EVALUATION 
    int empty_num = count_token(bucket, EMPTY, start, end);
    if (token_num == 4) {
        score += 100001;
    } else if (token_num == 3 && empty_num == 1) {
        
        score += 10;
    } else if (token_num == 2 && empty_num == 2) {
        score += 3;
    }
    
    if (opp_token_num == 4) {
        score -= 100000;
    }
    else if (opp_token_num == 3 && empty_num == 1) {
        score -= 11;
    }
    #else
    if (opp_token_num == 4) {
        score -= 100000;
    } else {
        if (token_num == 4) {
            score += 100000;
        }
        else if (token_num == 3) {
            score += 10;
        } else if (token_num == 2) {
            score += 5;
        }
    }
    #endif

    return score;
}