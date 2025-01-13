#include <stdlib.h>
#include <stdio.h>

//#include "antichess.h"
#include "agent.h"

char *filename = ".network";

// void get_position(char board[8][8], int player, double output[4096]) {  
//     input_layer(board, player, output); // move is a integer in range of 1...4096
// }

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

int main() {
    char board[8][8];
    double output[4096];
    initial_network_weights();
    initializeBoard(board);
    input_layer(board, 1, output);

    int max_position = -1;
    double max_value = 0;

    for (int i = 0; i < 4096; i++) {
        if (output[i] > max_value) {
            max_position = i;
            max_value = output[i];
        }
    }

    printf("%d: %f", max_position, max_value);
}