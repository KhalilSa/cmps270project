#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "globals.h"
#include "utils.h"
#include "board.h"
#include "bot.h"

/* initialize the connect four board with zeros (empty) */
void init_board(int** board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = EMPTY;
        }
    }
}

/* choose a column to drop the token in so that the token falls to the bottom of the column depending on the current state of the board */
void choose(int** board, int mode) {
    char column_user_input[MAX_INPUT];
    int col;
    while (1) {
        if (g_player == PLAYER || mode != PLAYER_VS_AI) {
            printf("\nChoose column: ");
            fgets(column_user_input, sizeof(column_user_input), stdin);
            trim_trailing(column_user_input);
            int valid = is_string_numeric(column_user_input);
            if (!valid) {
                puts("Invalid input, please enter an integer between 1-7.");
                continue;
            }
            
            col = atoi(column_user_input) - 1;

            if (col < 0 || col > 6) {
                puts("Invalid Column. please enter a column between 1-7.");
                continue;
            }

            if (board[0][col] != EMPTY) {
                puts("COLUMN IS FULL, Please choose another column.");
                continue;
            }
            fill_column(board, col, g_token);
        } else if (g_player == AI) {
            if (g_difficulty == RANDOM) {
                col = generate_random_int(0, 7);
                printf("Computer placed in column %d\n", col + 1);
                fill_column(board, col, g_token);
            } else {
                col = make_move(board);
                if (col != -1) {
                    printf("Computer placed in column %d\n", col + 1);
                    fill_column(board, col, g_token);
                }
            }
        }
        
        g_token = (g_token == RED) ? YELLOW : RED;
        g_player = (g_player == AI) ? PLAYER : AI;
        g_ai_piece = (g_ai_piece == RED) ? YELLOW : RED;
        g_player_piece = (g_player_piece == RED) ? YELLOW : RED;

        break;
    }
}

/* '
    Requires: column is a valid column number (0-6) and the column is not full (the top of the column is empty) 
    Effects: fills the column with the current token (1 or 2) and drops the token to the bottom of the column
*/
void fill_column(int **board, int column, int token) {
    for (int i = ROWS - 1; i >= 0; i--) {
        if (board[i][column] == EMPTY) {
            board[i][column] = token;
            break;
        }
    }               
}

/*  
    Requires: token is either 1 or 2 either representing red or yellow respectively 
    Effects: checks if the current player has won the game by checking all possible winning 
    combinations row, column, and diagonals then returns 1 if the player has won and 0 otherwise
*/
Boolean check_winner(int **board, int token) {

    // horizontal Checking
    for (int j = 0; j < COLS-3 ; j++ ) {
        for (int i = 0; i < ROWS; i++) {
            if (board[i][j] == token && board[i][j+1] == token && 
                board[i][j+2] == token && board[i][j+3] == token){
                return True;
            }
        }
    }

    // vertical Checking
    for (int i = 0; i<ROWS-3 ; i++ ) {
        for (int j = 0; j<COLS; j++) {
            if (board[i][j] == token && board[i+1][j] == token && 
                board[i+2][j] == token && board[i+3][j] == token){
                return True;
            }           
        }
    }

    // ascending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=0; j < COLS-3; j++) {
            if (board[i][j] == token && board[i-1][j+1] == token && 
                board[i-2][j+2] == token && board[i-3][j+3] == token)
                return True;
        }
    }
    // descending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=3; j < COLS; j++) {
            if (board[i][j] == token && board[i-1][j-1] == token && 
                board[i-2][j-2] == token && board[i-3][j-3] == token)
                return True;
        }
    }

    return False;       
}

/*
    Effects: returns the duration of the round
*/
double choose_with_timer(int **board, int mode, void (*choose)(int** board, int mode)) {
    time_t start = time(NULL);
    // callback for choose 
    choose(board, mode);
    double duration = (double)(time(NULL) - start);
    printf("took %.2f seconds", duration);
    return duration;
}