#include <stdlib.h>

#include "globals.h"
#include "board.h"
#include "utils.h"
#include "bot.h"

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

BestMove minimax(int **board, int depth, int alpha, int beta, Boolean is_maximazing_player) {
    Boolean *valid_locations = get_valid_locations(board);
    Boolean ai_won = check_winner(board, g_ai_piece);
    Boolean player_won = check_winner(board, g_player_piece);
    Boolean board_full = g_moves >= ROWS * COLS ? True : False;
    Boolean is_game_over = ai_won || player_won || board_full;
    int score;
    int column;
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

int score_board(int** board, int token) {
    int score = 0;
    // // Score Center Column
    // int center_column[ROWS];
    // int mid = COLS / 2;
    // for (int i = 0; i < ROWS; i++) {
    //     center_column[i] = board[i][mid];
    // }
    // int center_tokens_num = count_token(center_column, token, 0, ROWS);
    // score += center_tokens_num * 3;

    // Score Horizontal
    for (int i = 0; i < ROWS; i++) {
        // row of 7 elements
        int* row_array = board[i];
        for (int j = 0; j < COLS - 3; j++) {
            int start = j;
            int end = j + 4;
            // if (i < ROWS - 1) {
            //     for (int k = start; k < end; k++) {
            //         if (board[i+1][k] == EMPTY) continue;
            //     }
            // }
            int new_score = score_bucket(row_array, token, start, end);
            if (new_score <= -100000) return new_score;
            score += new_score;
        }
    }

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
            if (new_score <= -100000) return new_score;
            score += new_score;
        }
    }

    // score Ascending diagonals
    // ascending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=0; j < COLS-3; j++) {
            int diagonal_bucket[4];
            for (int k = 0; k < 4; k++) {
                diagonal_bucket[k] = board[i-k][j+k];
                // if (i != ROWS - 1 || k != 0) {
                //     if (board[i+k+1][j+k]) continue;
                // }
            }
            int new_score = score_bucket(diagonal_bucket, token, 0, 4);
            if (new_score <= -100000) return new_score;
            score += new_score;
            
        }
    }

    // descending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=3; j < COLS; j++) {
            int diagonal_bucket[4];
            for (int k = 0; k < 4; k++) {
                diagonal_bucket[k] = board[i-k][j-k];
                // if (i != ROWS - 1 || k != 0) {
                //     if (board[i+k+1][j+k]) continue;
                // }
            }
            int new_score = score_bucket(diagonal_bucket, token, 0, 4);
            if (new_score <= -100000) return new_score;
            score += new_score;
        }
    }
    return score;
}

int score_bucket(int* bucket, int token, int start, int end) {
    int opp_piece = g_player_piece;

    int score = 0;

    int token_num = count_token(bucket, token, start, end);
    int opp_token_num = count_token(bucket, opp_piece, start, end);
    #if OLD_EVALUATION 
    int empty_num = count_token(bucket, EMPTY, start, end);
    if (token_num == 3 && empty_num == 1) {
        score += 9;
    }
    if (opp_piece == 3 && empty_num == 1) {
        score -= 5;
    }
    if (token_num == 2 && empty_num == 2) {
        score += 3;
    } 
    if (opp_piece == 2 && empty_num == 2) {
        score -= 2;
    } 
    if (opp_piece == 1 && empty_num == 3) {
            score += 1;
    }
    if (opp_token_num == 1 && empty_num == 3) {
            score -= 1;
    }
    #else
    if (opp_token_num == 4) {
        score -= 100000;
    } else {
        if (token_num == 4) {
            score += 100000;
        }
        else if (token_num == 3) {
            score += 100;
        } else if (token_num == 2) {
            score += 1;
        }
    }
    #endif

    return score;
}


// /*
//  * Recursively solve a connect 4 position using negamax variant of min-max algorithm.
//  * @return the score of a position:
//  *  - 0 for a draw game
//  *  - positive score if you can win whatever your opponent is playing. Your score is
//  *    the number of moves before the end you can win (the faster you win, the higher your score)
//  *  - negative score if your opponent can force you to lose. Your score is the oposite of 
//  *    the number of moves before the end you will lose (the faster you lose, the lower your score).
//  */
// BestMove negamax(int** board, int moves_num, int alpha, int beta) {
//   if(moves_num == COLS * ROWS) // check for draw game
//     return (BestMove){-1, 0}; 

//   Boolean *valid_locations = get_valid_locations(board);
//     Boolean ai_won = check_winner(board, g_ai_piece);
//     Boolean player_won = check_winner(board, g_player_piece);
//     Boolean is_winning_move = ai_won || player_won;
//     int best_col = random_valid_column(valid_locations);
//   for(int x = 0; x < COLS; x++) // check if current player can win next move
//     if(valid_locations[x] && is_winning_move) 
//       return (BestMove){x, (COLS*ROWS+1 - moves_num)/2};

//   int max = (COLS*ROWS-1 - moves_num)/2;	// upper bound of our score as we cannot win immediately
//   if(beta > max) {
//     beta = max;                     // there is no need to keep beta above our max possible score.
//     if(alpha >= beta) return (BestMove){-1, beta};  // prune the exploration if the [alpha;beta] window is empty.
//   }
//   for(int x = 0; x < COLS; x++) // compute the score of all possible next move and keep the best one
//     if(valid_locations[x]) {
//       int** board_copy = (int**) alloca(ROWS * sizeof(int *));
//       for (int i = 0; i < ROWS; i++) {
//         board_copy[i] = (int *) alloca(COLS * sizeof(int));
//       }
//       int opp_piece = g_ai_piece;
//       if (g_token == g_ai_piece) {
//         opp_piece = g_player_piece;
//       }
//       copy_board(board, board_copy);
//       fill_column(board_copy, x, opp_piece);              // It's opponent turn in P2 position after current player plays x column.
//       int score = -negamax(board, g_moves + 1, -beta, -alpha).score; // explore opponent's score within [-beta;-alpha] windows:
//       // no need to have good precision for score better than beta (opponent's score worse than -beta)
//       // no need to check for score worse than alpha (opponent's score worse better than -alpha)

//       if(score >= beta) return (BestMove){x, score};  // prune the exploration if we find a possible move better than what we were looking for.
//       if(score > alpha) {alpha = score; best_col = x;}// reduce the [alpha;beta] window for next exploration, as we only 
//       // need to search for a position that is better than the best so far.ck of best possible score so far.
//     }

//   return (BestMove){best_col, alpha};
// }