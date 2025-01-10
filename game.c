#include <stdio.h>

#include "antichess.h"
#include "agent.h"

int main(){
    //8x8 char-array to emulate the chess board, using chess notation for the pieces (with p for pawns), and uppercase for white, lowercase for black
    char board[8][8]; 
    char legalMoves[100][5]; //array for legal moves on board position, length 100 because that should suffice
    clearLegalMoves(legalMoves);
    int player = 0; //variable for player whose turn it is, 0 for white, 1 for black
    initializeBoard(board);
    printBoardToTerminal(board);
    return 0;
}