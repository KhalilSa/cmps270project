#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "bool.h"
#include "utils.h"
#include "globals.h"

// String/UI functions  

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

Boolean is_string_numeric(char *str) {
    if (str == NULL) {
        return False;
    }
    int i = 0;
    // check if the string only includes digits
    while (str[i] != '\0') {
        if (!isdigit(str[i])) return False;
        i++;
    }

    return True;
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
Boolean check_valid_name(char *name) {
    if (name == NULL || name[0] == '\n') {
        return False;
    }
    int i = 0;
    // check if the string only includes digits
    while (name[i] != '\0') {
        if (isspace(name[i])) return False;
        i++;
    }

    return True;
}

// helper functions

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

/* copies a board's data from another board */
void copy_board(int** src_board, int** dist_board) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            dist_board[i][j] = src_board[i][j];
        }
    }
}


/* converts string to lowercase */
void string_to_lower_case(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

/* count number of token in a given window */
int count_token(const int* arr, int token, int start, int end) {
    int count = 0;
    for (int i = start; i < end; i++) {
        if (arr[i] == token) {
            count++;
        }
    }
    return count;
}