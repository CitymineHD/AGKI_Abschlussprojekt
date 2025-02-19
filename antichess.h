#include <stdbool.h>

void initializeBoard(char board[8][8]);

void printBoardToTerminal(char board[8][8]);

int determineLegalMoves(bool moves[4096], int player, char board[8][8]);

void get_position(char board[8][8], int player, double output[4096]);

void add_to_ReplayMemory(int move);

void save_ReplayMemory(int player);