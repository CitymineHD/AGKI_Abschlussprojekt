#include <stdio.h>

#include "antichess.h"

int main(){
    //8x8 char-array to emulate the chess board, using chess notation for the pieces (with p for pawns), and upper- vs lowercase letters to differenciate black and white
    char board[8][8];
    initializeBoard(board);
    printBoardToTerminal(board);
    return 0;
}