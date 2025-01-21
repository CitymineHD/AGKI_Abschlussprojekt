#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

// #include "antichess.h"


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
    printf("  ---------------------------------\n");
    for (int i = 7; i >= 0; i--){
        printf("%d |",i+1);
        for (int j = 0; j < 8; j++){
            printf(" %c |", board[i][j]);
        }
        printf("\n  ---------------------------------\n");
    }
    printf("    A   B   C   D   E   F   G   H\n");
}

void clearMoves (bool moves[4096]){
    for (int i = 0; i < 4096; i++){
        moves[i] = false;
    }
}

int moveToInt(char a, char b, char c, char d){
    return a * 8 * 8 * 8 + b * 8 * 8 + c * 8 + d;
}

int determineLegalMoves( bool moves[4096], int player, char board[8][8], bool *_isHittingMove){ //_isHittingMove saves the type of move
    //determines legal moves in position of board for player (1 for white, 0 for black) and writes them into a bool array (moves)
    //every 4096 values of the bool array stands for a move where the first two letters is the i (1-8) and the j (A-H) location of the piece and
    // the next two letters are for the i and j possition of the aim of the piece
    //for ease: castle not allowed, no en passent, pawns just vanish on promotion    
    int numbersOfAllPossibleMoves = 0;

    int OPTIONS = 4096; // Constant for all Moves
    //bool moves[OPTIONS]; // Saves every possible move for one player at a direct position
    for (int i = 0; i< OPTIONS; i++){ // Set every value on false;
        moves[i] = false;
    }

    bool _acceptAllMoves = true;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            char piece = board[i][j] - player * 32; //maps lowecases to uppercases if player==1, so we can write only cases for uppercase letters
            //only the pieces of the player who has the turn get checked
            switch (piece){
                case 'P':
                    //Pawns can only move in one direction (not the BoyBand)
                    //therefor a seperated check for each player
                    if (!player){ // black
                        //checks if pawn can move 1 forward and 2 forward if its on line 7 
                        if (board[i-1][j] == ' '){
                            if (_acceptAllMoves){
                                moves[moveToInt(i,j,i-1,j)]  = true;
                                numbersOfAllPossibleMoves++;
                                if (i == 6){
                                    if (board[i-2][j] == ' '){
                                        moves[moveToInt(i,j,i-2,j)] = true;
                                        numbersOfAllPossibleMoves++;
                                    }
                                }
                            }
                        }
                        //checks if the pawn can captcher another piece
                        if (j+1 <= 7 && board[i-1][j+1] >= 'a' - player * 32 && board[i-1][j+1] <= 'z' - player * 32){
                            if (_acceptAllMoves){ // Only clears all Moves once at the first time a take move got located.
                                _acceptAllMoves = false;
                                clearMoves(moves);
                                numbersOfAllPossibleMoves = 0;
                            }

                            moves[moveToInt(i,j,i-1,j+1)]  = true;
                            numbersOfAllPossibleMoves++;
                        }

                        if (j-1 >= 0 && board[i-1][j-1] >= 'a' - player * 32 && board[i-1][j-1] <= 'z' - player * 32){
                            if (_acceptAllMoves){
                                _acceptAllMoves = false;
                                clearMoves(moves);
                                numbersOfAllPossibleMoves = 0;
                            }

                            moves[moveToInt(i,j,i-1,j-1)]  = true;
                            numbersOfAllPossibleMoves++;
                        }
                    } else { //white
                        if (board[i+1][j] == ' '){
                            if (_acceptAllMoves){
                                moves[moveToInt(i,j,i+1,j)]  = true;
                                numbersOfAllPossibleMoves++;
                                if (i == 1){
                                    if (board[i+2][j] == ' '){
                                        moves[moveToInt(i,j,i+2,j)]  = true;
                                        numbersOfAllPossibleMoves++;
                                    }
                                }
                            }
                        }
                        //checks if the pawn can captcher another piece
                        if (j+1 <=7 && board[i+1][j+1] >= 'a' - player * 32 && board[i+1][j+1] <= 'z' - player * 32){
                            if (_acceptAllMoves){ // Only clears all Moves once at the first time a take move got located.
                                _acceptAllMoves = false;
                                clearMoves(moves);
                                numbersOfAllPossibleMoves = 0;
                            }

                            moves[moveToInt(i,j,i+1,j+1)]  = true;
                            numbersOfAllPossibleMoves++;
                        }

                        if (j-1 >= 0 && board[i+1][j-1] >= 'a' - player * 32 && board[i+1][j-1] <= 'z' - player * 32){
                            if (_acceptAllMoves){
                                _acceptAllMoves = false;
                                clearMoves(moves);
                                numbersOfAllPossibleMoves = 0;
                            }

                            moves[moveToInt(i,j,i+1,j-1)]  = true;
                            numbersOfAllPossibleMoves++;
                        }
                    }

                    break;
                case 'N':
                    //direction
                    for (int d = 1; d < 9; d++){
                        //Knight moves are 2 fields in one direction and 1 in the other direction 90 Degrie to the first direction 
                        //Locats the knights moves
                        int iDirection, jDirection;        
                        iDirection = d % 4 == 0 ? -1 : d % 4 == 1 ? 1 : d % 4 == 2 ? 2 : -2;
                        if (d < 5){ // gives the right j values for the i values so the knight can jump 2 forward and 1 sidewards
                            jDirection = d % 4 == 0 ? 2 : d % 4 == 1 ? -2 : d % 4 == 2 ? -1 : 1;
                        } else { //mirrors the first half
                            jDirection = d % 4 == 0 ? -2 : d % 4 == 1 ? 2 : d % 4 == 2 ? 1 : -1;
                        }
                        
                        //Checks if the Move is inside the board
                        if ((i + iDirection < 0  || i + iDirection > 7) ||(j + jDirection < 0 || j + jDirection > 7)){
                            continue;
                        }
                        //checks if the located position is empty
                        if (board[i + iDirection][j + jDirection] == ' '){
                            if (_acceptAllMoves){
                                moves[moveToInt(i,j,i + iDirection, j + jDirection)]  = true;
                                numbersOfAllPossibleMoves++;
                            }
                        // checks if the piece on the position is an enemy or an ally
                        } else {
                            if (board[i + iDirection][j + jDirection] >= 'a' - player * 32 && board[i + iDirection][j + jDirection] <= 'z' - player * 32){
                                if (_acceptAllMoves){
                                    _acceptAllMoves = false;
                                    clearMoves(moves);
                                    numbersOfAllPossibleMoves = 0;
                                }

                                moves[moveToInt(i,j,i + iDirection,j + jDirection)]  = true;
                                numbersOfAllPossibleMoves++;
                            }

                        }
                    }
                           
                    break;
                case 'R':
                case 'B':
                case 'Q':
                case 'K':
                    int iD, jD;
                    //direction
                    for (int d = 1; d < 9 ; d++){ //its starts at 1 and end at 8 so its easier for the % 3 opperation to locate the direction
                        //Locates the Direction of the piece (Goes in every Direction around the piece)
                        int iDirection, jDirection;

                        iDirection = d % 3 == 0 ? 0 : d % 3 == 1 ? 1 : -1;
                        jDirection = d / 3 == 0 ? 0 : d / 3 == 1 ? 1 : -1;
                        
                        int movesInDirection = piece == 'K' ? 2 : 10; //The King can only Move 1 square in every Direction
                            
                        //Checks that the right piece only moves in the right direction
                        if ((piece == 'B') && (d > 0 && d < 4 || d == 6 )){
                            continue;
                        }

                        if ((piece == 'R') && (d > 3 && d < 9 && d != 6)){
                            continue;
                        }

                        for (int r = 1; r < movesInDirection; r++){ // range
                            iD = i + (r * iDirection);
                            jD = j + (r * jDirection);
                            if ((iD < 0 || iD > 7) || (jD < 0 || jD > 7)){ //Checks if the Move is on the board
                                break;
                            } else if (board[iD][jD] == ' '){
                                if (_acceptAllMoves){
                                    moves[moveToInt(i,j,iD, jD)]  = true;
                                    numbersOfAllPossibleMoves++;
                                }
                            } else {
                                if (board[iD][jD] >= 'a' - player * 32 && board[iD][jD] <= 'z' - player * 32){
                                    if (_acceptAllMoves){
                                        _acceptAllMoves = false;
                                        clearMoves(moves);
                                        numbersOfAllPossibleMoves = 0;
                                    }

                                    moves[moveToInt(i,j,iD,jD)]  = true;
                                    numbersOfAllPossibleMoves++;
                                }

                                break; //After the piece hit a piece the loop should end
                            }
                        }
                    }

                    break;
            }
        }
    }

    //saves the type of move // capter move or no capture move
    *_isHittingMove = !_acceptAllMoves;

    return numbersOfAllPossibleMoves;
}

int decToOct(int dec){//decimal number to octal number
    int oct[4];
    for (int i = 3; i >= 0; i--){
        oct[i] = dec % 8;
        dec /= 8;
    }

    int erg = oct[0] * 1000 + oct[1] * 100 + oct[2] * 10 + oct[3];
    return erg;
}

void splitMoveIntoSingleDigits(int moveData[4], int move){
    //quick and dirty
    moveData[0] = move / 1000;
    moveData[1] = (move / 100) % 10;
    moveData[2] = (move / 10) % 10;
    moveData[3] = move % 10;
}

void updateBoard(char board[8][8], int move){ // move value between 0 and 4095
    //transform the decimal int to an octal int and splits i  into its single digits
    int moveData[4];
    splitMoveIntoSingleDigits(moveData, decToOct(move));

    //Perform the Move
    board[moveData[2]][moveData[3]] = board[moveData[0]][moveData[1]];
    board[moveData[0]][moveData[1]] = ' ';

    //remove pawns after reaching the end (no Promotion just vanished)
    if (board[moveData[2]][moveData[3]] == 'p' && moveData[2] == 7){
        board[moveData[2]][moveData[3]] = ' ';
    }

    if (board[moveData[2]][moveData[3]] == 'P' && moveData[2] == 0){
        board[moveData[2]][moveData[3]] = ' ';
    }
}
    
int moveFromPlayer(char *input){
    //only possible notation is for Example: e2e4 (No Capital Letters);
    scanf("%4s",input);
    if (strlen(input) == 4){
        return moveToInt(input[1]-'1',input[0]-'a',input[3]-'1',input[2]-'a');
    }

    return -1;
}


void AiVsAiNoBrain(int howManyMoves){
    //every move is just the first legal move who was found
    bool moves[4096];

    char board[8][8];
    initializeBoard(board);
    int anz = 1; int moveCount = 0;
    bool egal = false; //dont need this here but its nessesary for the method determineLegalMoves
    while (anz!= 0 && moveCount <= howManyMoves){
        if (moveCount % 2){
            anz = determineLegalMoves(moves, 0, board, &egal);
        } else {
            anz = determineLegalMoves(moves, 1, board, &egal);
        }
        
        printf("------------------Zug: %d ----------------------\n",moveCount);
        printBoardToTerminal(board);
        printf("Move: %d\tSpieler: %s\tZüge: %d\t",moveCount, moveCount % 2 ? "Schwarz" : "Weiß",anz);
        for (int i=0; i<4096; i++){
            if (moves[i]){
                updateBoard(board, i);
                printf("Next Move: %d\n", decToOct(i));
                break;
            }
        }

        moveCount++;
    }

    printf("\n");
    
}

bool checkMove(bool moves[4096], int move){
    //if the move is in the moveslist return true else false
    for (int i = 0; i < 4096; i++)
    {
        if (moves[i] && i == move){
            return true;
        }
    }

    return false;
}

void printAllPossibleMoves(bool moves[4096]){
    //TODO
    for (int i = 0; i < 4096; i++)
    {
        if (moves[i]){
            printf("%d\n", decToOct(i));
        }
    }
    // printf("Noch nicht verfügbar sry :< ");
}

void playerVsBadAi(bool color){ //true = Weiß || false = Schwarz
    //starting color is the color of the player
    //the color switches each move and determen if the player or the Ai is moving
    bool moves[4096];
    char board[8][8];
    initializeBoard(board);
    int anz = -1; //anz possible moves
    int move = -1; 
    int badAi = color ? 0 : 1;
    int player = color ? 1 : 0;
    bool winner; //true = Player won || false = Ai won
    bool passt = false;
    bool egal = false; //dont need this information but its necessary
    while (anz != 0){
        printBoardToTerminal(board);
        if (color){ //player
            anz = determineLegalMoves(moves, player, board, &egal);
            if (anz != 0){
                while (!passt){
                    char input[100];
                    move = moveFromPlayer(input);
                    if (move >= 0 && move <= 4096 && checkMove(moves, move)){
                        passt = true;
                    } else {
                        printf("Nicht gültiger Zug!\n");
                        printAllPossibleMoves(moves); //prints all Legal Moves as humanReadable Input
                    }
                }

                passt = false;
            } else {
                winner = true;
                break;
            }
            
            updateBoard(board,move);
        } else { //BadAi
        //TODO
            int possibleMovesCount = determineLegalMoves(moves, badAi, board, &egal);
            printAllPossibleMoves(moves);
            if (possibleMovesCount == 0){
                winner = false;
                break;
            }

            for (int i = 0; i < 4096; i++){
                if (moves[i]){
                    updateBoard(board,i);
                    break;
                }
            }                
        }

        color = !color;
    }
}

// main is only for testing
// int main(){

//     // AiVsAiNoBrain(10);
//     playerVsBadAi(true);
    
// }






