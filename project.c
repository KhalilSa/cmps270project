#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define nRows    6
#define nCols    7

int tokens[nRows][nCols];
char frame[] = "-----------------------------";
int column = 0;
int token = 1;
int emp = 0;

void empty_slots();
void print_slots();
void table();
void choose();
void fill_column();
int check_winner(int token);

int main() {
    empty_slots();
    print_slots();
    table();
    char name1[20]; char name2[20];
    printf("Enter first player's name: ");
    scanf("%s", name1);
    printf("\nEnter second player's name: ");
    scanf("%s", name2);
    int lower = 1, upper = 2;
    srand(time(0));
        int num = (rand() % (upper - lower + 1)) + lower;
        token = num;
    while (1) {
        printf("\n%s's turn.\n", (token == 1) ? name1 : name2);
        choose();
        printf("\n\n");
        table();
        if (check_winner(1)) {
            printf("\n\%s is the winner!\n\n", name1);
            break;  
        }
        if (check_winner(2)) {
            printf("\n\%s is the winner!\n\n", name2);
            break;  
        }   
    }
    
    return 0; 
}

void empty_slots() {
    int i, j;
    
    for (i = 0; i < nRows; i++) {
        for (j = 0; j < nCols; j++) {
            tokens[i][j] = emp;
        }
    }
}

void print_slots() {
    int i, j;
    
    for (i = 0; i < nRows; i++) {
        for (j = 0; j < nCols; j++) {
            printf("%d ", tokens[i][j]);
        }
        printf("\n");
    }
}

void table() {
    printf("%s\n", frame);
    
    int i, j;
    
    for (i = 0; i < nRows; i++) {
        for (j = 0; j < nCols; j++) {
            printf("| %d ", tokens[i][j]);
        }
        printf("|\n");
    }
        printf("%s\n", frame);
}

void choose() {
    int c;
    
    while (1) {
        printf("\nChoose column: ");
        scanf(" %d", &c);
        switch(c) {
            case 1:
                column = 0;
                break;
            case 2:
                column = 1;
                break;
            case 3:
                column = 2;
                break;
            case 4:
                column = 3;
                break;
            case 5:
                column = 4;
                break;
            case 6:
                column = 5;
                break;
            case 7:
                column = 6;
                break;
            default:
                column = 999;
                printf("\nInvalid input! Try again.\n\n");
                table();
        }
        if ((column >= 0 && column <= 6) && (tokens[0][column] == emp)) {
            fill_column();
            token = (token == 1) ? 2 : 1;
            break;
        }
    }
}

void fill_column() {
    int cRow ; 

    for (cRow = nRows-1; cRow >= 0; cRow--) {
        if (tokens[cRow][column] == emp) {
            tokens[cRow][column] = token;
            break;
        }   
    }               
}

int check_winner(int token) {
     
    // horizontal Checking
    for (int j = 0; j<nRows-3 ; j++ ){
        for (int i = 0; i<nCols; i++){
            if (tokens[i][j] == token && tokens[i][j+1] == token && 
                tokens[i][j+2] == token && tokens[i][j+3] == token){
                return 1;
            }           
        }
    }
    // vertical Checking
    for (int i = 0; i<nRows-3 ; i++ ){
        for (int j = 0; j<nCols; j++){
            if (tokens[i][j] == token && tokens[i+1][j] == token && 
                tokens[i+2][j] == token && tokens[i+3][j] == token){
                return 1;
            }           
        }
    }
    
    // ascending Diagonal Check
    for (int i=3; i < nRows; i++){
        for (int j=0; j < nCols-3; j++){
            if (tokens[i][j] == token && tokens[i-1][j+1] == token && 
                tokens[i-2][j+2] == token && tokens[i-3][j+3] == token)
                return 1;
        }
    }
    // descending Diagonal Check
    for (int i=3; i < nRows; i++){
        for (int j=3; j < nCols; j++){
            if (tokens[i][j] == token && tokens[i-1][j-1] == token && 
                tokens[i-2][j-2] == token && tokens[i-3][j-3] == token)
                return 1;
        }
    }

    return 0;       
}
    

void print_slots() {
    int i, j;
    
    for (i = 0; i < nRows; i++) {
        for (j = 0; j < nCols; j++) {
            printf("%d ", tokens[i][j]);
        }
        printf("\n");
    }
}

void table() {
    printf("%s\n", frame);
    
    int i, j;
    
    for (i = 0; i < nRows; i++) {
        for (j = 0; j < nCols; j++) {
            printf("| %d ", tokens[i][j]);
        }
        printf("|\n");
    }
        printf("%s\n", frame);
}

void choose() {
    int c;
    
    while (1) {
        printf("\nChoose column: ");
        scanf(" %d", &c);
        switch(c) {
            case 1:
                column = 0;
                break;
            case 2:
                column = 1;
                break;
            case 3:
                column = 2;
                break;
            case 4:
                column = 3;
                break;
            case 5:
                column = 4;
                break;
            case 6:
                column = 5;
                break;
            case 7:
                column = 6;
                break;
            default:
                column = 999;
                printf("\nInvalid input! Try again.\n\n");
                table();
        }
        if ((column >= 0 && column <= 6) && (tokens[0][column] == emp)) {
            fill_column();
            token = (token == 1) ? 2 : 1;
            break;
        }
    }
}

void fill_column() {
    int cRow ; 

    for (cRow = nRows-1; cRow >= 0; cRow--) {
        if (tokens[cRow][column] == emp) {
            tokens[cRow][column] = token;
            break;
        }   
    }               
}

int check_winner(int token) {
     
    // horizontal Checking
    for (int j = 0; j<nRows-3 ; j++ ){
        for (int i = 0; i<nCols; i++){
            if (tokens[i][j] == token && tokens[i][j+1] == token && 
                tokens[i][j+2] == token && tokens[i][j+3] == token){
                return 1;
            }           
        }
    }
    // vertical Checking
    for (int i = 0; i<nRows-3 ; i++ ){
        for (int j = 0; j<nCols; j++){
            if (tokens[i][j] == token && tokens[i+1][j] == token && 
                tokens[i+2][j] == token && tokens[i+3][j] == token){
                return 1;
            }           
        }
    }
    
    // ascending Diagonal Check
    for (int i=3; i < nRows; i++){
        for (int j=0; j < nCols-3; j++){
            if (tokens[i][j] == token && tokens[i-1][j+1] == token && 
                tokens[i-2][j+2] == token && tokens[i-3][j+3] == token)
                return 1;
        }
    }
    // descending Diagonal Check
    for (int i=3; i < nRows; i++){
        for (int j=3; j < nCols; j++){
            if (tokens[i][j] == token && tokens[i-1][j-1] == token && 
                tokens[i-2][j-2] == token && tokens[i-3][j-3] == token)
                return 1;
        }
    }

    return 0;       
}
    