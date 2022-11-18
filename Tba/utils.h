#ifndef UTILS_H
#define UTILS_H

#include "bool.h"

// String/UI functions  

void print_board(int** board);
void trim_trailing(char *str);
Boolean is_string_numeric(char *str);
void print_dashed_line(const unsigned int N);
Boolean check_valid_name(char *name);

// helper functions

int generate_random_int(int lower, int upper);
void copy_board(int** src_board, int** dist_board);
void string_to_lower_case(char* str);
int count_token(const int* arr, int token, int start, int end);

#endif