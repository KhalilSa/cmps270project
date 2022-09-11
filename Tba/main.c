#include <stdio.h>
#include <string.h>
#include "header.h"

int main(int argc, char **argv) {
	char name1[MAX_NAME], name2[MAX_NAME];

	// prompting user for players' name
	printf("Enter first player name: ");
	fgets(name1, MAX_NAME, stdin);

	printf("Enter second player name: ");
	fgets(name2, MAX_NAME, stdin);

	// removing trailing new line character
	name1[strcspn(name1, "\n")] = 0;
    name2[strcspn(name2, "\n")] = 0;

	printf("Welcome %s and %s to 4-connect!!\n", name1, name2);
	printf("Generating 7x6 grid (classic) ...\n");

	print_dashed_line();

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COLUMN; j++) {
            printf("|0");
        }
        printf("|\n");
    }

	print_dashed_line();

	return 0;
}

void print_dashed_line() {
    for (int i = 0; i < COLUMN * 2 + 1; i++) {
        printf("-");
	}
	printf("\n");
}
