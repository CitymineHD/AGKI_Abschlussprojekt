#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "antichess.h"
#include "agent.h"

//playerColor -> true = Weiß || false = Schwarz
int playerVsAi(bool playerColor, char* filename){
    srand((unsigned int)time(NULL)); //initializing pseudorandom numbers
    //initializing and reading network arrays
    double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons];
    double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons];
    double threshold[Number_of_Layer-1][Number_of_Output_Neurons];
    double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons];
    printf("Reading file... ");
    fflush(stdout);
    readFromFile(filename, network_weights_input, network_weights_output, threshold);
    printf("File read!\n");

    //starting color is the color of the player
    //the color switches each move and determen if the player or the Ai is moving
    bool moves[4096];
    char board[8][8];
    initializeBoard(board);
    
    //information for the determineLegalMoves()
    int aiColour = playerColor ? 0 : 1;
    int player = playerColor ? 1 : 0;
    bool hittingMove = false; // draw after 50 moves without hitting 
    int drawTime = 0;

    int anz = -1; //anz possible moves
    int playerMove = -1;
    int winner = 0; // 0 = draw || 1 = player won || -1 = AI won
    bool passt = false;
    bool showSoftmax = true;
    double distribution[Number_of_Output_Neurons]; //distribution for network moves
    
    while (anz != 0 && drawTime <= 50){
        printBoardToTerminal(board);
        if (playerColor){ //player
            anz = determineLegalMoves(moves, player, board, &hittingMove);
            hittingMove ? drawTime = 0 : drawTime++;
            if (anz != 0){
                while (!passt){
                    char input[100]; //input for handling commands instead of moves
                    playerMove = moveFromPlayer(input);
                    if (playerMove >= 0 && playerMove <= 4096 && checkMove(moves, playerMove)){
                        passt = true;
                    } else if (strcmp(input, "softmax")==0){
                        if (showSoftmax==false){
                            printf("showing softmax distribution on next AI move. Now please make your move.\n");
                            showSoftmax = true;
                        }else{
                            printf("n longer showing softmax distribution on next AI move. Now please make your move.\n");
                            showSoftmax = false;
                        }
                    }else {
                        printf("Nicht gültiger Zug!\n");
                        printAllPossibleMoves(moves); //prints all Legal Moves as humanReadable Input
                    }
                }

                passt = false;
            } else {
                winner = 1;
                break;
            }
            
            updateBoard(board,playerMove);
        } else { //AI moves
            int possibleMovesCount = determineLegalMoves(moves, aiColour, board, &hittingMove);
            hittingMove ? drawTime = 0 : drawTime++;
            run_network(board, aiColour, network_weights_input, network_weights_output, threshold, activated_neurons);
            softmax(distribution, activated_neurons, moves, 5); //testing relatively low amplifier to allow for exploration
            if (showSoftmax){
                printf("This is the softmax-distribution for the AI for this move:\n");
                for (int i = 0; i < Number_of_Output_Neurons; i++){
                    if (distribution[i]!=0){
                        printf("Move %d has chance %f to be played.\n", decToOct(i), distribution[i]);
                    }
                }
            }
            int aiMove = determineMove(distribution); //choose random move according to softmaxed distribution
            printf("AI plays move %d\n", decToOct(aiMove));
            //catching errors (hopefully irrellevant)
            if (aiMove < 0 || aiMove > Number_of_Output_Neurons-1){
                fprintf(stderr, "something went very, very wrong\n");
                exit(-1);
            }else if (!moves[aiMove]){
                fprintf(stderr, "Network wants to do illegal move for some reason :(\n");
                exit(-1);
            }
            updateBoard(board,aiMove);           
        }

        playerColor = !playerColor;
    }

    return winner;
}


int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "Please give filename of AI to test\n");
        return -1;
    }
    int outcome = playerVsAi(true, argv[1]);
    if (outcome == 1){
        printf("You win!\n");
    }else if (outcome == -1){
        printf("You lose!\n");
    }else {
        printf("It's a draw!\n");
    }
    return 0;
}