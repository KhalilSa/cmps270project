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

#define AI_PIECE 1
#define PLAYER_PIECE 2

enum MODE {PLAYER_VS_PLAYER, PLAYER_VS_AI};
enum TOKEN {EMPTY, RED, YELLOW};
enum AGENT {PLAYER, AI};
enum AI_DIFFICULTY {RANDOM, EASY = 2, MEDIUM = 4, HARD = 6};

int column = 0;
char token = RED;
int player = PLAYER;
int difficulty = RANDOM;

// Main functions

void init_board(int** board);
void choose(int** board, int mode);
void fill_column(int **board, int column, int player);
int check_winner(int **board, char token);
double choose_with_timer(int **board, int mode, void (*choose)(int** board, int mode));

// String/UI functions  

void print_board(int** board);
void trim_trailing(char *str);
int is_string_numeric(char *str);
void print_dashed_line(const unsigned int N);
int check_valid_name(char *name);

// helper functions

int generate_random_int(int lower, int upper);
void copy_board(int** board, int** board_copy);
void string_to_lower_case(char* str);

// Bot functions

void minimax(int **board, int depth, int alpha, int beta, int is_maximazing_player, int *column, int *score);
int* get_valid_locations(int **board);
int score_board(int** board, int player);
int random_valid_column(int* valid_columns);
int get_open_row(int** board, int column);

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
    player = generate_random_int(PLAYER, AI);
    if (mode == PLAYER_VS_AI) {
        if (player == AI) {
            strncpy(name2, name1, MAX_INPUT);
            strncpy(name1, "Computer", MAX_INPUT);
        } else {
            strncpy(name2, "Computer", MAX_INPUT);
        }
    }
    

    // track how many moves the players have made so far
    unsigned int moves = 0;
    double total_time_red = 0;
    double total_time_yellow = 0;

    // now initilize the board and print it before the game starts
    init_board(board);
    print_board(board);

    // main game loop
    while (1) {
        // announce the winner in case of a tie (the board is full but no winner)
        // the winner in that case is the one who took less time
        if (moves >= ROWS * COLS) {
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
        printf("\n%s's turn.\n", (token == 1) ? name1 : name2);
        if (token == RED) {
            total_time_red += choose_with_timer(board, mode, choose);
        } else if (token == YELLOW) {
            total_time_yellow += choose_with_timer(board, mode, choose);
        }
        
        printf("\n\n");

        print_board(board);
        if (check_winner(board, 1)) {
            printf("\n\%s is the winner!\n\n", name1);
            break;  
        }
        if (check_winner(board, 2)) {
            printf("\n\%s is the winner!\n\n", name2);
            break;  
        }

        moves++;
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
    while (1) {
        if (player == PLAYER || mode != PLAYER_VS_AI) {
            printf("\nChoose column: ");
            fgets(column_user_input, sizeof(column_user_input), stdin);
            trim_trailing(column_user_input);
            int valid = is_string_numeric(column_user_input);
            if (!valid) {
                puts("Invalid input, please enter an integer between 1-7.");
                continue;
            }
            
            int col = atoi(column_user_input) - 1;

            if (col < 0 || col > 6) {
                puts("Invalid Column. please enter a column between 1-7.");
                continue;
            }

            if (board[0][col] != EMPTY) {
                puts("COLUMN IS FULL, Please choose another column.");
                continue;
            }
            fill_column(board, col, token);
        } else if (player == AI) {
            if (difficulty == RANDOM) {
                int col = generate_random_int(0, 7);
                printf("Computer placed in column %d\n", col);
                fill_column(board, col, token);
            }
        }
        
        token = (token == RED) ? YELLOW : RED;
        player = (player == AI) ? PLAYER : AI;
        break;
    }
}

/* '
    Requires: column is a valid column number (0-6) and the column is not full (the top of the column is empty) 
    Effects: fills the column with the current token (1 or 2) and drops the token to the bottom of the column
*/
void fill_column(int **board, int column, int player) {
    for (int i = ROWS - 1; i >= 0; i--) {
        if (board[i][column] == EMPTY) {
            board[i][column] = player;
            break;
        }
    }               
}

/*  
    Requires: token is either 1 or 2 either representing red or yellow respectively 
    Effects: checks if the current player has won the game by checking all possible winning 
    combinations row, column, and diagonals then returns 1 if the player has won and 0 otherwise
*/
int check_winner(int **board, char token) {

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

// /*
//     Makes an intellegent move
// */
// int make_move(int **board) {
//     int column;
//     int score;
//     minimax(**board, depth, alpha, beta, AI_PIECE, &column, &score);
//     return column;
// }

void minimax(int **board, int depth, int alpha, int beta, int is_maximazing_player, int *column, int *score) {
    int *valid_locations = get_valid_locations(board);
    int ai_won = check_winner(board, AI_PIECE);
    int player_won = check_winner(board, PLAYER_PIECE);
    int is_game_over = ai_won || player_won;
    if (depth == 0 || is_game_over) {
        if (is_game_over) { 
            if (ai_won) {
                *column = -1;
                *score  = 1000;
            }
            else if (player_won) {
                *column = -1;
                *score  = -1000;
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
            *score  = score_board(board, AI_PIECE);
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
                fill_column(board_copy, *column, AI_PIECE);
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
                fill_column(board_copy, *column, PLAYER_PIECE);
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
    }
    return random_col;
}

void copy_board(int** board, int** board_copy) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board_copy[i][j] = board[i][j];
        }
    }
}

int score_board(int** board, int player) {
    return 0;
}