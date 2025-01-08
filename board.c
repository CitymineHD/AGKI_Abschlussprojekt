#include <stdio.h>

#include "antichess.h"

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

void clearLegalMoves (char legalMoves[100][5]){
    //puts EOS into all entries of legalMoves
    for (int i = 0; i < 100; i++){
        legalMoves[i][0] = 0;
    }
}

int determineLegalMoves(char legalMoves[100][5], int player, char board[8][8]){
    //determines legal moves in position of board for player (0 for white, 1 for black) and writes them into LegalMoves array
    //writes all moves into legalMoves first, then clears if it finds a capture move and only includes capture moves from then on
    //for ease: castle not allowed, no en passent, pawns just vanish on promotion
    int out = 0
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            switch (board[i][j]-player*32){ //maps lowecases to uppercases if player==1, so we can write only cases for uppercase letters
                case P: //TODO
                    break;
                case R: //TODO
                    break;
                case N: //TODO
                    break;
                case B: //TODO
                    break;
                case Q: //TODO
                    break;
                case K: //TODO
                    break;
            }
        }
    }

}