#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "antichess.h"
#include "agent.h"

int main(void){
    srand((unsigned int)time(NULL)); //initializing pseudorandom numbers
    char board[8][8]; 
    int player = 1; //variable for player whose turn it is, 1 for white, 0 for black
    initializeBoard(board);
    double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons];
    double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons];
    double threshold[Number_of_Layer-1][Number_of_Output_Neurons];

    double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons];
    bool legal_moves[Number_of_Output_Neurons];
    double eta = 0.002; //0.02 seems stable and reasonably fast, gotta test for differnt uses in actual training
    double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons];    //array for the summed up gradients for weights between input and hidden layer and hidden and output layer (respectively), needs to be multiplied by eta and then added to the actual network
    double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons];  //order is [intoNeuronNumber][outOfNeuronNumber]
    double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons];            //same, but for biases, [layerNumber][neuronNumber], layer 0 is first hidden layer
    initializeDeltas(deltaInputWeights, deltaOutputWeights, deltaThresholds);
    initial_network_weights(network_weights_input, network_weights_output, threshold);
    //readFromFile(".trainedNetwork", network_weights_input, network_weights_output, threshold);
    //run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    //printf("RunLucas says: %f\n", activated_neurons[1][796]);
    runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    bool _isHittingMove;
    determineLegalMoves(legal_moves, player, board, &_isHittingMove);
    int numProp = 1000; //number of testing backprops
    int outcome = 1;
    //printf("this is test. pls take a moment to pray this works.\n               neuron for e2e4:             %f, other neuron at %f\n", activated_neurons[1][796], activated_neurons[1][405]);
    for (int j = 0; j < numProp; j++){
        printf("after %d      backprop step, move neuron fires at %f, other legal neuron at %f and illegal at %f\n", j, activated_neurons[1][796], activated_neurons[1][405], activated_neurons[1][0]);
        backpropStep(board, player, 796, outcome, network_weights_input, network_weights_output, threshold, activated_neurons, legal_moves, deltaInputWeights, deltaOutputWeights, deltaThresholds);
        //printf("RunLucas says: %f\n", activated_neurons[1][796]);
        applyDeltas(network_weights_input, network_weights_output, threshold, deltaInputWeights, deltaOutputWeights, deltaThresholds, eta);
        //run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        //printf("RunLucas says: %f\n", activated_neurons[1][796]);
        runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        //outcome *= -1;

    }
    double distribution[Number_of_Output_Neurons];
    softmax(distribution, activated_neurons, legal_moves, 10);
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        if (distribution[i]!=0) {
            printf("Move %d has probability %f\n", i, distribution[i]);
        }
    }
    /*for (int i = 0; i < 5000; i++){
        int move = determineMove(distribution);
        if (move != 796)
        printf("Choosing move %d\n", move);
    }*/
    //writeToFile(".testNetwork", network_weights_input, network_weights_output, threshold);
    return 0;
}