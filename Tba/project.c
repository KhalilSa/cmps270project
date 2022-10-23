#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define ROWS 6
#define COLS 7
#define MAX_INPUT 20
#define EMPTY 0

int board[ROWS][COLS];
int column = 0;
char token = 1;

void init_board();
void print_board();
void choose();
void fill_column(int column);
int check_winner(char token);
void trim_trailing(char *str);
int is_string_numeric(char *str);
void print_dashed_line(const unsigned int N);
int check_valid_name(char *name);
int generate_random_int(int lower, int upper);
double choose_with_timer(void (*choose)());

int main() {
    init_board();
    print_board();

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
    
    // generate random number (token)
    // 1 represents red, 2 represents yellow
    token = generate_random_int(1, 2);

    // track how many moves the players have made so far
    unsigned int moves = 0;
    double total_time_red = 0;
    double total_time_yellow = 0;

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

        printf("\n%s's turn.\n", (token == 1) ? name1 : name2);

        if (token == 1) {
            total_time_red += choose_with_timer(choose);
        } else {
            total_time_yellow += choose_with_timer(choose);
        }
        printf("\n\n");

        print_board();
        if (check_winner(1)) {
            printf("\n\%s is the winner!\n\n", name1);
            break;  
        }
        if (check_winner(2)) {
            printf("\n\%s is the winner!\n\n", name2);
            break;  
        }

        moves++;
    }
    return 0; 
}

// initialize the connect four board with zeros (empty) 
void init_board() {
    for (unsigned int i = 0; i < ROWS; i++) {
        for (unsigned int j = 0; j < COLS; j++) {
            board[i][j] = EMPTY;
        }
    }
}

// print the connect four board with the current state of the game 
void print_board() {
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
void choose() {
    char column_user_input[MAX_INPUT];
    while (1) {
        printf("\nChoose column: ");
        fgets(column_user_input, sizeof(column_user_input), stdin);
        trim_trailing(column_user_input);
        int valid = is_string_numeric(column_user_input);
        if (!valid) {
            puts("Invalid input, please enter an integer between 1-7.");
            continue;
        }
        
        int column = atoi(column_user_input) - 1;

        if (column < 0 || column > 6) {
            puts("Invalid Column. please enter a column between 1-7.");
            continue;
        }

        if (board[0][column] != EMPTY) {
            puts("COLUMN IS FULL, Please choose another column.");
			continue;
        }

        fill_column(column);
        token = (token == 1) ? 2 : 1;
        break;
    }
}

/* '
    Requires: column is a valid column number (0-6) and the column is not full (the top of the column is empty) 
    Effects: fills the column with the current token (1 or 2) and drops the token to the bottom of the column
*/
void fill_column(int column) {
    for (unsigned int i = ROWS - 1; i >= 0; i--) {
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
int check_winner(char token) {

    // horizontal Checking
    for (int j = 0; j<ROWS-3 ; j++ ){
        for (int i = 0; i<COLS; i++){
            if (board[i][j] == token && board[i][j+1] == token && 
                board[i][j+2] == token && board[i][j+3] == token){
                return 1;
            }

        }
    }
    // vertical Checking
    for (int i = 0; i<ROWS-3 ; i++ ){
        for (int j = 0; j<COLS; j++){
            if (board[i][j] == token && board[i+1][j] == token && 
                board[i+2][j] == token && board[i+3][j] == token){
                return 1;
            }           
        }
    }

    // ascending Diagonal Check
    for (int i=3; i < ROWS; i++){
        for (int j=0; j < COLS-3; j++){
            if (board[i][j] == token && board[i-1][j+1] == token && 
                board[i-2][j+2] == token && board[i-3][j+3] == token)
                return 1;
        }
    }
    // descending Diagonal Check
    for (int i=3; i < ROWS; i++){
        for (int j=3; j < COLS; j++){
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
    unsigned int index = strlen(str) - 1;
    if (str == NULL  || index == 0) {
        return;
    }
    
    while (index >=0 && isspace(str[index])) {
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
double choose_with_timer(void (*choose)()) {
    time_t start = time(NULL);
    // callback for choose 
    choose();
    double duration = (double)(time(NULL) - start);
    printf("took %.2f seconds", duration);
    return duration;
}