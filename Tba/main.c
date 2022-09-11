#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "header.h"

int main(int argc, char **argv) {
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
	printf("Welcome %s and %s to 4-connect!!\n", name1, name2);
	printf("Generating 7x6 4-connect grid (classic) ...\n");

	// initialize the board
	initialize_board(board);
	// print the board
	print_board(board);

	// toss a fair coin
	bool result = rand() % 2;
	if (result) {
        printf("%s will start", name1);
    } else {
        printf("%s will start", name2);
    }

	return 0;
}

void print_dashed_line(void) {
    for (int i = 0; i < COLUMN * 2 + 1; i++) {
        printf("-");
	}
	printf("\n");
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


