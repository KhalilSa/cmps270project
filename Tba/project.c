#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

// macros

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define ROWS 6
#define COLS 7
#define MAX_INPUT 20

enum MODE {PLAYER_VS_PLAYER, PLAYER_VS_AI};
enum TOKEN {EMPTY, RED, YELLOW};
enum AGENT {PLAYER, AI};
enum AI_DIFFICULTY {RANDOM, EASY = 2, MEDIUM = 4, HARD = 6};

int g_column = 0;
int g_token = RED;
int g_player = PLAYER;
int g_difficulty = EASY;
int g_ai_piece, g_player_piece;

// track how many moves the players have made so far
unsigned int g_moves = 0;

// Main functions

void init_board(int** board);
void choose(int** board, int mode);
void fill_column(int **board, int column, int player);
int check_winner(int **board, int token);
double choose_with_timer(int **board, int mode, void (*choose)(int** board, int mode));

// String/UI functions  

void print_board(int** board);
void trim_trailing(char *str);
int is_string_numeric(char *str);
void print_dashed_line(const unsigned int N);
int check_valid_name(char *name);

// helper functions

int generate_random_int(int lower, int upper);
void copy_board(int** src_board, int** dist_board);
void string_to_lower_case(char* str);
int count_token(int* arr, int token, int start, int end);

// Bot functions

int make_move(int **board);
void minimax(int **board, int depth, int alpha, int beta, int is_maximazing_player, int *column, int *score);
int* get_valid_locations(int **board);
int score_board(int** board, int token);
int random_valid_column(int* valid_columns);
int score_bucket(int* bucket, int token, int start, int end);

int main() {
    // Allocate board in the stack
    // this shouldn't cause any problem as long as ROWS and COLS are small numbers
    int** board = (int**) alloca(ROWS * sizeof(int *));
    for (int i = 0; i < ROWS; i++) {
        board[i] = (int *) alloca(COLS * sizeof(int));
    }

    // get players' names
    char name1[MAX_INPUT]; char name2[MAX_INPUT];

    while (1) {
        printf("\nEnter first player's name: ");
        fgets(name1, sizeof(name1), stdin);
        trim_trailing(name1);
        int valid = check_valid_name(name1);
        if (valid) break;
        printf("Invalid name! name should not have spaces and can't start with \\n.");
    }

    // AI vs Player Mode or Player vs Player Mode
    char which_mode[MAX_INPUT];
    int mode = PLAYER_VS_PLAYER;
    while (1) {
        printf("Do you want to play againt the computer?[Y/N]: ");
        fgets(which_mode, sizeof(which_mode), stdin);
        trim_trailing(which_mode);
        string_to_lower_case(which_mode);
        if (strcmp(which_mode, "yes") == 0 || strcmp(which_mode, "y") == 0) {
            mode = PLAYER_VS_AI;
            break;
        }
        else if (strcmp(which_mode, "no") == 0 || strcmp(which_mode, "n") == 0) break;
        else {
            printf("Invalid output! Answer should be yes or no...\n");
        }
    }
    
    if (mode == PLAYER_VS_PLAYER) {
        while (1) {
            printf("\nEnter second player's name: ");
            fgets(name2, sizeof(name2), stdin);
            trim_trailing(name2);
            // validate name
            int valid = check_valid_name(name2);
            if (valid) {
                if (strcmp(name1, name2) != 0) break;
                puts("That name is already taken. Enter an another name.");
            } else {
                puts("Invalid name! name should not have spaces and can't start with \\n.");
            }
        }
    }

    print_dashed_line(40);
    printf("\tMode: %s", mode == PLAYER_VS_PLAYER ? "Player vs Player\n" : "Player vs AI\n");
    print_dashed_line(40);
    
    // Randomly pick the one who starts first
    g_player = generate_random_int(PLAYER, AI);
    if (mode == PLAYER_VS_AI) {
        if (g_player == AI) {
            strncpy(name2, name1, MAX_INPUT);
            strncpy(name1, "Computer", MAX_INPUT);
            g_player_piece = YELLOW;
            g_ai_piece = RED;
        } else {
            strncpy(name2, "Computer", MAX_INPUT);
            g_player_piece = RED;
            g_ai_piece = YELLOW;
        }
    }
    
    double total_time_red = 0;
    double total_time_yellow = 0;

    // now initilize the board and print it before the game starts
    init_board(board);
    print_board(board);

    // main game loop
    while (1) {
        // announce the winner in case of a tie (the board is full but no winner)
        // the winner in that case is the one who took less time
        if (g_moves >= ROWS * COLS) {
            if (total_time_red == total_time_yellow) {
                printf("It's a tie. both players took %.2f seconds\n", total_time_red);
            } else {
                int less_time = (total_time_red < total_time_yellow);
                printf("%s won. ", less_time ? name1 : name2);
                printf("(%s took %.2f seconds, %s took %.2f seconds)\n", name1, total_time_red, name2, total_time_yellow);
            }
            break;
        }
        // Gameplay Code
        printf("\n%s's turn.\n", (g_token == RED) ? name1 : name2);
        if (g_token == RED) {
            total_time_red += choose_with_timer(board, mode, choose);
        } else if (g_token == YELLOW) {
            total_time_yellow += choose_with_timer(board, mode, choose);
        }
        
        printf("\n\n");

        print_board(board);
        if (check_winner(board, RED)) {
            printf("\n\%s is the winner!\n\n", name1);
            break;  
        }
        if (check_winner(board, YELLOW)) {
            printf("\n\%s is the winner!\n\n", name2);
            break;  
        }

        g_moves++;
    }
    return 0; 
}

// initialize the connect four board with zeros (empty) 
void init_board(int** board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = EMPTY;
        }
    }
}

// print the connect four board with the current state of the game 
void print_board(int** board) {
    print_dashed_line(2 * COLS + 1);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("|%d", board[i][j]);
        }
        printf("|\n");
    }
    print_dashed_line(2 * COLS + 1);
}

// choose a column to drop the token in so that the token falls to the bottom of the column depending on the current state of the board
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
int check_winner(int **board, int token) {

    // horizontal Checking
    for (int j = 0; j < COLS-3 ; j++ ) {
        for (int i = 0; i < ROWS; i++) {
            if (board[i][j] == token && board[i][j+1] == token && 
                board[i][j+2] == token && board[i][j+3] == token){
                return 1;
            }
        }
    }

    // vertical Checking
    for (int i = 0; i<ROWS-3 ; i++ ) {
        for (int j = 0; j<COLS; j++) {
            if (board[i][j] == token && board[i+1][j] == token && 
                board[i+2][j] == token && board[i+3][j] == token){
                return 1;
            }           
        }
    }

    // ascending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=0; j < COLS-3; j++) {
            if (board[i][j] == token && board[i-1][j+1] == token && 
                board[i-2][j+2] == token && board[i-3][j+3] == token)
                return 1;
        }
    }
    // descending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=3; j < COLS; j++) {
            if (board[i][j] == token && board[i-1][j-1] == token && 
                board[i-2][j-2] == token && board[i-3][j-3] == token)
                return 1;
        }
    }

    return 0;       
}

/*
    Requires: str is a string with a trailing newline character that needs to be removed
    Effects: removes the trailing newline character from the string
*/
void trim_trailing(char *str) {
    if (str == NULL) return;
    int index = strlen(str) - 1;
    if (index <= 0) {
        return;
    }
    
    while (index >= 0 && isspace(str[index])) {
        str[index] = '\0';
        index--;
    }
}

/*
    Requires: a string
    Effects: Check if a string has only numeric characters and returns 1 if it does and 0 otherwise
*/

int is_string_numeric(char *str) {
    if (str == NULL) {
        return 0;
    }
    int i = 0;
    // check if the string only includes digits
    while (str[i] != '\0') {
        if (!isdigit(str[i])) return 0;
        i++;
    }

    return 1;
}

/*
    Requires: n is the number of dashes to print
    Effects: prints n dashes to the screen and a newline character at the end 
*/
void print_dashed_line(const unsigned int N) { 
    for (unsigned int i = 0; i < N; i++) {
        printf("-");
    }
	printf("\n");
}

/*
    Requires: char *name is a string representing the name of the player and name 
    should not have spaces or a newline character and can not be a number
    Effects: returns 1 if the name is valid and 0 otherwise
*/
int check_valid_name(char *name) {
    if (name == NULL || name[0] == '\n') {
        return 0;
    }
    int i = 0;
    // check if the string only includes digits
    while (name[i] != '\0') {
        if (isspace(name[i])) return 0;
        i++;
    }

    return 1;
}

/*
    Requires: lower and upper are valid integers and lower < upper
    Effects: returns a random integer between lower and upper 
*/
int generate_random_int(int lower, int upper) {
    // initialize a unique seed
    srand(time(0));
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
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

// converts string to lowercase
void string_to_lower_case(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

/*
    Makes an intellegent move
*/
int make_move(int **board) {
    int column;
    int score;
    minimax(board, g_difficulty, -__INT_MAX__, __INT_MAX__, g_ai_piece, &column, &score);
    // int *valid_locations = get_valid_locations(board);
    // int best_score = 0;
    // int best_col = random_valid_column(valid_locations);
    // for (int col = 0; col < COLS; col++) {
    //     if (!valid_locations[col]) continue;
    //     int** board_copy = (int**) alloca(ROWS * sizeof(int *));
    //     for (int i = 0; i < ROWS; i++) {
    //         board_copy[i] = (int *) alloca(COLS * sizeof(int));
    //     }
    //     copy_board(board, board_copy);
    //     fill_column(board_copy, col, g_ai_piece);
    //     int score = score_board(board_copy, g_ai_piece);
    //     if (score > best_score) {
    //         best_score = score;
    //         best_col = col;
    //     }
    // }
    // return best_col;
    return column;
}

void minimax(int **board, int depth, int alpha, int beta, int is_maximazing_player, int *column, int *score) {
    int *valid_locations = get_valid_locations(board);
    int ai_won = check_winner(board, g_ai_piece);
    int player_won = check_winner(board, g_player_piece);
    int board_full = g_moves >= ROWS * COLS ? 1 : 0;
    int is_game_over = ai_won || player_won || board_full;
    if (depth == 0 || is_game_over) {
        if (is_game_over) { 
            if (ai_won) {
                *column = -1;
                *score  = 10000;
            }
            else if (player_won) {
                *column = -1;
                *score  = -10000;
            } 
            // draw
            else {
                *column = 0;
                *score = 0;
            }
        }
        // depth = 0
        else {
            *column = -1;
            *score  = score_board(board, g_ai_piece);
        }
        return;
    }

    if (is_maximazing_player) {
        *score = -__INT32_MAX__;
        *column = random_valid_column(valid_locations);
        for (int col = 0; col < COLS; col++) {
            if (valid_locations[col]) {
                int** board_copy = (int**) alloca(ROWS * sizeof(int *));
                for (int i = 0; i < ROWS; i++) {
                    board_copy[i] = (int *) alloca(COLS * sizeof(int));
                }
                copy_board(board, board_copy);
                fill_column(board_copy, *column, g_ai_piece);
                int new_score = 0;
                int new_column = 0;
                minimax(board_copy, depth - 1, alpha, beta, 0, &new_column, &new_score);
                if (new_score > *score) {
                    *score = new_score;
                    *column = col;
                }
                alpha = max(alpha, *score);
                if (alpha >= beta) break;
            }
        }
    }
    else {
       *score = __INT32_MAX__;
       *column = random_valid_column(valid_locations);
        for (int col = 0; col < COLS; col++) {
            if (valid_locations[col]) {
                int** board_copy = (int**) alloca(ROWS * sizeof(int *));
                for (int i = 0; i < ROWS; i++) {
                    board_copy[i] = (int *) alloca(COLS * sizeof(int));
                }
                copy_board(board, board_copy);
                fill_column(board_copy, *column, g_player_piece);
                int new_score = 0;
                int new_column = 0;
                minimax(board_copy, depth - 1, alpha, beta, 1, &new_column, &new_score);
                if (new_score < *score) {
                    *score = new_score;
                    *column = col;
                }
                alpha = max(alpha, *score);
                if (alpha >= beta) break;
            }
        }
    }
}

int* get_valid_locations(int **board) {
    static int valid_columns[COLS];
    for (int i = 0; i < COLS; i++) {
        // if the column still not full mark it as valid (1) location, otherwise mark it as invalid (0)
        if (board[0][i] == 0) valid_columns[i] = 1;
        else valid_columns[i] = 0;
    }
    return valid_columns;
}

int random_valid_column(int* valid_columns) {
    int random_col = generate_random_int(0, COLS);

    // return random col if it's valid
    if (valid_columns[random_col]) return random_col;

    // if not return the closest valid column
    int i = 0;
    while (i < COLS || !valid_columns[random_col]) {
        random_col++;
        random_col %= COLS;
        i++;
    }
    return random_col;
}

void copy_board(int** src_board, int** dist_board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            dist_board[i][j] = src_board[i][j];
        }
    }
}

int score_board(int** board, int token) {
    int score = 0;
    // Score Center Column
    int center_column[ROWS];
    for (int i = 0; i < ROWS; i++) {
        int mid = COLS / 2;
        center_column[i] = board[i][mid];
    }
    int center_tokens_num = count_token(center_column, token, 0, ROWS);
    score += center_tokens_num * 6;
    // Score Horizontal
    for (int i = 0; i < ROWS; i++) {
        int* row_array = board[i];
        for (int j = 0; j < COLS - 3; j++) {
            int start = j;
            int end = j + 4;
            score += score_bucket(row_array, token, start, end);
        }
    }

    // Score Vertical
    for (int j = 0; j < COLS; j++) {
        int col_array[ROWS];
        for (int i = 0; i < ROWS; i++) {
            col_array[i] = board[i][j];
        }
        for (int i = 0; i < ROWS - 3; i++) {
            int start = j;
            int end = j + 4;
            score += score_bucket(col_array, token, start, end);
        }
    }

    // score Ascending diagonals
    // ascending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=0; j < COLS-3; j++) {
            int diagonal_bucket[4];
            for (int k = 0; k < 4; k++) {
                diagonal_bucket[k] = board[i-k][j+k];
            }
            score += score_bucket(diagonal_bucket, token, 0, 4);
            
        }
    }

    // descending Diagonal Check
    for (int i=3; i < ROWS; i++) {
        for (int j=3; j < COLS; j++) {
            int diagonal_bucket[4];
            for (int k = 0; k < 4; k++) {
                diagonal_bucket[k] = board[i-k][j-k];
            }
            score += score_bucket(diagonal_bucket, token, 0, 4);
        }
    }
    return score;
}

int score_bucket(int* bucket, int token, int start, int end) {
    int opp_piece = g_player_piece;
    if (token == g_player_piece) {
        opp_piece = g_ai_piece;
    }
    int score = 0;

    int token_num = count_token(bucket, token, start, end);
    int empty_num = count_token(bucket, EMPTY, start, end);
    int opp_token_num = count_token(bucket, opp_piece, start, end);
    if (token_num == 4) {
        score += 10000;
    }
    else if (token_num == 3 && empty_num == 1) {
        score += 10;
    }
    else if (token_num == 2 && empty_num == 2) {
        score += 5;
    }
    else if (opp_token_num == 3 && empty_num == 1) {
        score -= 100;
    }

    return score;
}

int count_token(int* arr, int token, int start, int end) {
    int count = 0;
    for (int i = start; i < end; i++) {
        if (arr[i] == token) {
            count++;
        }
    }
    return count;
}