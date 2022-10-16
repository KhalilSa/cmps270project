#ifndef header
#define header

#define MAX_NAME 20
#define ROW 6
#define COLUMN 7

void print_dashed_line(void);
void print_board(char board[ROW][COLUMN]);
void initialize_board(char board[ROW][COLUMN]);

void trim_trailing(char *str);
int is_string_numeric(char *str);


#endif
