#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "header.h"

int main() {
	char name1[MAX_NAME], name2[MAX_NAME];
	char board[ROW][COLUMN];
	time_t t;

	// initialize random number generator
	srand((unsigned) time(&t));

	// prompting user for players' name
	printf("Enter first player name: ");
	fgets(name1, MAX_NAME, stdin);

	printf("Enter second player name: ");
	fgets(name2, MAX_NAME, stdin);

	// removing trailing new line character
	name1[strcspn(name1, "\n")] = 0;
    name2[strcspn(name2, "\n")] = 0;

    // welcoming messages
	printf("Welcome %s and %s to connect-4!!\n", name1, name2);
	printf("Generating 7x6 connect-4 grid (classic) ...\n");

	// initialize the board
	initialize_board(board);
	// print the board
	print_board(board);

	// toss a fair coin
	bool first_toss = rand() % 2;
	if (first_toss) {
        printf("%s will start as red!\n", name1);
		printf("Next turn will be %s's turn as yellow.\n", name2);
    } else {
        printf("%s will start as red!\n", name2);
		printf("Next turn will be %s's turn as yellow.\n", name1);
    }

	char column_user_input[2];
	while (true) {
		printf("Enter an integer between 1-7: ");
		fgets(column_user_input, 2, stdin);
		trim_trailing(column_user_input);
		int valid = is_string_numeric(column_user_input);
		if (!valid) {
			puts("Input Invalid, please enter an integer between 1-7.");
			continue;
		}
		int column = atoi(column_user_input) + 1;
		if (board[0][column] != '0') {
			puts("COLUMN IS FULL, Please choose another column.");
			continue;
		}

		break;

	}

	return 0;
}

void print_dashed_line(void) {
	printf("---------------\n");
}

void print_board(char board[ROW][COLUMN]) {
    print_dashed_line();
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("|%c", board[i][j]);
        }
        printf("|\n");
    }
    print_dashed_line();
}

void initialize_board(char board[ROW][COLUMN]) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            board[i][j] = '0';
        }
    }
}



/*
    removes trailing whitespaces and newlines from a string
*/
void trim_trailing(char *str) {
    unsigned int index = strlen(str) - 1;
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
    int i = 0;
    // check if the rest of the string includes only digits
    while (str[i] != '\0') {
        if (!isdigit(str[i])) return 0;
        i++;
    }

    return 1;
}

