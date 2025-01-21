#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "antichess.h"
#include "agent.h"

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr, "please provide the following arguments: fileToRead, fileToWrite, numGamesToTrain\n");
        return -1;
    }
    srand((unsigned int)time(NULL)); //initializing pseudorandom numbers
    double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons];
    double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons];
    double threshold[Number_of_Layer-1][Number_of_Output_Neurons];

    double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons];
    double eta = 0.01; //lets try 0.01
    double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons];    //array for the summed up gradients for weights between input and hidden layer and hidden and output layer (respectively), needs to be multiplied by eta and then added to the actual network
    double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons];  //order is [intoNeuronNumber][outOfNeuronNumber]
    double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons];            //same, but for biases, [layerNumber][neuronNumber], layer 0 is first hidden layer
    initializeDeltas(deltaInputWeights, deltaOutputWeights, deltaThresholds);
    printf("Reading file... ");
    fflush(stdout);
    readFromFile(argv[1], network_weights_input, network_weights_output, threshold);
    printf("File read!\n");
    fflush(stdout);
    //training loop

    for (int numGames = 0; numGames < atoi(argv[3]); numGames++){
        printf("Starting game %d... ", numGames);
        fflush(stdout);
        bool legalMoves[Number_of_Output_Neurons];
        double distribution[Number_of_Output_Neurons];
        int *movesMade = malloc(sizeof(int)*1000);
        int capacity = 1000;
        int numMoves = 0;
        int timesNotCaptured = 0;
        int player = 1; //variable for player whose turn it is, 1 for white, 0 for black
        int outcome;
        char board[8][8]; 
        initializeBoard(board);
        bool isCaptureMove;
        //loop for one game
        while((determineLegalMoves(legalMoves, player, board, &isCaptureMove)!=0) && (timesNotCaptured < 100)){
            //run until a player has no legal moves or until we didn't capture 1000 times (draw by repitition)
            run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
            softmax(distribution, activated_neurons, legalMoves, 5); //testing relatively low amplifier to allow for exploration
            int currentMove = determineMove(distribution); //choose random move according to softmaxed distribution
            //catching errors (hopefully irrellevant)
            if (currentMove < 0 || currentMove > Number_of_Output_Neurons-1){
                fprintf(stderr, "something went very, very wrong\n");
                goto trashGame; //trashing game, starting next one in for loop
                //return -1;
            }else if (!legalMoves[currentMove]){
                fprintf(stderr, "Network wants to do illegal move for some reason :(\n");
                goto trashGame; //trashing game, starting next one in for loop
                //return -1;
            }
            updateBoard(board, currentMove); //actually perform move
            //dynamic allocation for replay memory
            if (numMoves<capacity){
                movesMade[numMoves] = currentMove;
            }else{
                movesMade = realloc(movesMade, sizeof(int)*capacity*2);
                capacity += capacity;
            }
            numMoves++; //increment move counter
            //change player
            if(player == 0){
                player = 1;
            }else{
                player = 0;
            }
            //update draw by repitition counter
            if (!isCaptureMove){
                timesNotCaptured++;
            }else{
                timesNotCaptured = 0;
            }
        }
        //catch draw by repitition
        if (timesNotCaptured == 100){
            printf(" ---Draw by repitition--- ");
            outcome = 0;
            numMoves = numMoves - 80; //disregard most of the repeating moves in this case as to not train weirdly
        }else if(player == 1){
            printf(" ---White won!--- ");
            outcome = 1; //if white won, we want the outcome to start at win
        }else{
            printf(" ---Black won!--- ");
            outcome = -1; //if black won, the initial outcome is a loss, because white starts in backprop
            player = 1; //reset player to white
        }
        initializeBoard(board); //reset board
        printf(" ...backpropping... ");
        fflush(stdout);
        for (int movesPropped = 0; movesPropped < numMoves; movesPropped++){
            backpropStep(board, player, movesMade[movesPropped], outcome, network_weights_input, network_weights_output, threshold, activated_neurons, legalMoves, deltaInputWeights, deltaOutputWeights, deltaThresholds);
            applyDeltas(network_weights_input, network_weights_output, threshold, deltaInputWeights, deltaOutputWeights, deltaThresholds, eta);
            //switch player and outcome
            if (player == 1){
                player = 0;
            }else{
                player = 1;
            }
            outcome *= -1;
            updateBoard(board, movesMade[movesPropped]); //apply move
            /*system("clear");
            printBoardToTerminal(board);
            sleep(1);*/
        }
        printf("done!\n");
        fflush(stdout);
        trashGame:
        free(movesMade);
        if ((numGames%100 == 0) && (numGames != 0)){
            printf("Writing file... ");
            fflush(stdout);
            writeToFile(argv[2], network_weights_input, network_weights_output, threshold);
            printf("File written!\n");
        }
    }


    writeToFile(argv[2], network_weights_input, network_weights_output, threshold);
    return 0;
}