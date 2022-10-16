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
void print_dashed_line(const int N);
int generate_random_int(int lower, int upper);
double choose_with_timer(void (*choose)());

int main() {
    init_board();
    print_board();

    // get players' names
    char name1[MAX_INPUT]; char name2[MAX_INPUT];
    printf("Enter first player's name: ");
    fgets(name1, sizeof(name1), stdin);
    trim_trailing(name1);

    while (1) {
        printf("\nEnter second player's name: ");
        fgets(name2, sizeof(name2), stdin);
        trim_trailing(name2);
        // validate name
        if (strcmp(name1, name2) != 0) {
            break;
        }
        puts("That name is already taken. Enter an another name.");
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

void init_board() {
    for (unsigned int i = 0; i < ROWS; i++) {
        for (unsigned int j = 0; j < COLS; j++) {
            board[i][j] = EMPTY;
        }
    }
}

void print_board() {
    print_dashed_line(2 * COLS + 1);
    for (char i = 0; i < ROWS; i++) {
        for (char j = 0; j < COLS; j++) {
            printf("|%d", board[i][j]);
        }
        printf("|\n");
    }
    print_dashed_line(2 * COLS + 1);
}

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

void fill_column(int column) {
    for (unsigned int i = ROWS - 1; i >= 0; i--) {
        if (board[i][column] == EMPTY) {
            board[i][column] = token;
            break;
        }
    }               
}

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
    removes trailing whitespaces and newlines from a string
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
    Check if a string has only numeric character
    @returns 1 if the string is numeric
             0 otherwise
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

void print_dashed_line(const int N) { 
    for (unsigned int i = 0; i < N; i++) {
        printf("-");
    }
	printf("\n");
}

// generates random integer between lower and upper
int generate_random_int(int lower, int upper) {
    // initialize a unique seed
    srand(time(0));
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

double choose_with_timer(void (*choose)()) {
    time_t start = time(NULL);
    //Do your operations here
    choose();
    double duration = (double)(time(NULL) - start);
    printf("took %.2f seconds", duration);
    return duration;
}