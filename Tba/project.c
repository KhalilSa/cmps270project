#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "project.h"

// Global Variables

int g_column = 0;
int g_token = RED;
int g_player = PLAYER;
int g_difficulty = EASY;
int g_ai_piece, g_player_piece;

// track how many moves the players have made so far
unsigned int g_moves = 0;

int main() {
    // Allocate board in the stack
    // this shouldn't cause any problem as long as ROWS and COLS are small numbers
    int** board = (int**) alloca(ROWS * sizeof(int *));
    for (int i = 0; i < ROWS; i++) {
        board[i] = (int *) alloca(COLS * sizeof(int));
    }

    // get players' names
    char name1[MAX_INPUT]; char name2[MAX_INPUT];

    while (True) {
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
    while (True) {
        printf("Do you want to play againt the computer?[Y/N]: ");
        fgets(which_mode, MAX_INPUT, stdin);
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

    if (mode == PLAYER_VS_AI) {
        char difficulty[MAX_INPUT];
        printf("Difficulties\n");
        printf("1. Random\t 2. Easy\t 3. Medium\t 4. Hard\n");
        while (True) {
            printf("Choose AI difficulty (default=Easy): \n");
            fgets(difficulty, MAX_INPUT, stdin);
            if (strcmp(difficulty, "\n") == 0) break;
            trim_trailing(difficulty);
            string_to_lower_case(difficulty);
            if (strcmp(difficulty, "random") == 0 || strcmp(difficulty, "1") == 0) {
                g_difficulty = RANDOM;
            } else if (strcmp(difficulty, "easy") == 0 || strcmp(difficulty, "2") == 0) {
                g_difficulty = EASY;
            } else if (strcmp(difficulty, "medium") == 0 || strcmp(difficulty, "3") == 0) {
                g_difficulty = MEDIUM;
            } else if (strcmp(difficulty, "hard") == 0 || strcmp(difficulty, "4") == 0) {
                g_difficulty = HARD;
            } else {
                printf("Invalid Input, choose one of the difficulties or hit enter to choose the default.");
                continue;
            }
            break;
        }
    }
    
    
    if (mode == PLAYER_VS_PLAYER) {
        while (True) {
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
    while (True) {
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