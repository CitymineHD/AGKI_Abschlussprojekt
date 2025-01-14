#include <stdlib.h>
#include <stdio.h>

//#include "antichess.h"
#include "agent.h"

char *filename = ".network";

void get_position(char board[8][8], int player, double output[4096]) {  
    run_network(board, player);
}
