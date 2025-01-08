#include <stdio.h>

//setting up the pieces in starting position
void initializeBoard(char board[8][8]){
    //empty board
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            board[i][j] = ' ';
        }
    }
    //pawns
    for (int i = 0; i < 8; i++){
        board[1][i] = 'p';
        board[6][i] = 'P';
    }
    //other stuff without loop, kinda ugly but has to be done
    board[0][0] = 'r';
    board[0][1] = 'n';
    board[0][2] = 'b';
    board[0][3] = 'q';
    board[0][4] = 'k';
    board[0][5] = 'b';
    board[0][6] = 'n';
    board[0][7] = 'r';
    board[7][0] = 'R';
    board[7][1] = 'N';
    board[7][2] = 'B';
    board[7][3] = 'Q';
    board[7][4] = 'K';
    board[7][5] = 'B';
    board[7][6] = 'N';
    board[7][7] = 'R';
}

void printBoardToTerminal(char board[8][8]){
    printf("---------------------------------\n");
    for (int i = 0; i < 8; i++){
        printf("|");
        for (int j = 0; j < 8; j++){
            printf(" %c |", board[i][j]);
        }
        printf("\n---------------------------------\n");
    }
}

int main(){
    //8x8 char-array to emulate the chess board, using chess notation for the pieces (with p for pawns), and upper- vs lowercase letters to differenciate black and white
    char board[8][8];
    initializeBoard(board);
    printBoardToTerminal(board);
    return 0;
}