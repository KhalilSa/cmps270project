#ifndef GLOBALS_H
#define GLOBALS_H

// macros

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

// Enums

enum MODE {PLAYER_VS_PLAYER, PLAYER_VS_AI};
enum TOKEN {EMPTY, RED, YELLOW};
enum AGENT {PLAYER, AI};
enum AI_DIFFICULTY {RANDOM, EASY = 2, MEDIUM = 4, HARD = 6};

#define ROWS 6
#define COLS 7
#define MAX_INPUT 20

#define DEBUG 0
#define OLD_EVALUATION 0

// Global Variables

extern int g_column;
extern int g_token;
extern int g_player;
extern int g_difficulty;
extern int g_ai_piece, g_player_piece;

// track how many moves the players have made so far
extern unsigned int g_moves;

#endif