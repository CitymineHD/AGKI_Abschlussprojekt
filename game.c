#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "antichess.h"
#include "agent.h"

int main(void){
    //8x8 char-array to emulate the chess board, using chess notation for the pieces (with p for pawns), and uppercase for white, lowercase for black
    char board[8][8]; 
    int player = 1; //variable for player whose turn it is, 1 for white, 0 for black
    initializeBoard(board);
    double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons];
    double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons];
    double threshold[Number_of_Layer-1][Number_of_Output_Neurons];

    double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons];

    bool legal_moves[Number_of_Output_Neurons];
    srand((unsigned int)time(NULL));
    int weightToCheck = rand()%Number_of_Hidden_Neurons;
    double eta = 0;
    double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons];    //array for the summed up gradients for weights between input and hidden layer and hidden and output layer (respectively), needs to be multiplied by eta and then added to the actual network
    double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons];  //order is [intoNeuronNumber][outOfNeuronNumber]
    double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons];            //same, but for biases, [layerNumber][neuronNumber], layer 0 is first hidden layer
    initializeDeltas(deltaInputWeights, deltaOutputWeights, deltaThresholds);
    initial_network_weights(network_weights_input, network_weights_output, threshold);
    //run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    printf("RunLucas says: %f\n", activated_neurons[1][796]);
    runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    determineLegalMoves(legal_moves, player, board);
    printf("this is test. pls take a moment to pray this works.\n               neuron for e2e4:             %f, bias is %.20f and weight %d is %.20f\n", activated_neurons[1][796], threshold[1][796], 0, network_weights_output[0][0]);
    for (int j = 0; j < 1; j++){
        backpropStep(board, player, 796, 1, network_weights_input, network_weights_output, threshold, activated_neurons, legal_moves, deltaInputWeights, deltaOutputWeights, deltaThresholds);
        writeToFile("testNetwork", network_weights_input, network_weights_output, threshold);
        printf("RunLucas says: %f\n", activated_neurons[1][796]);
        applyDeltas(network_weights_input, network_weights_output, threshold, deltaInputWeights, deltaOutputWeights, deltaThresholds, eta);
        writeToFile("testNetwork-4ApplyDeltas", network_weights_input, network_weights_output, threshold);
        //run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        printf("RunLucas says: %f\n", activated_neurons[1][796]);
        runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        writeToFile("testNetwork-3", network_weights_input, network_weights_output, threshold);
        printf("after %d      backprop step, neuron fires at %f, bias is %0.20f and weight %d is %.20f\n", j, activated_neurons[1][796], threshold[1][796], 0, network_weights_output[0][0]);
    }
    for (int j = 0; j < 1; j++){
        backpropStep(board, player, 796, 1, network_weights_input, network_weights_output, threshold, activated_neurons, legal_moves, deltaInputWeights, deltaOutputWeights, deltaThresholds);
        writeToFile("testNetwork-2", network_weights_input, network_weights_output, threshold);
        printf("RunLucas says: %f\n", activated_neurons[1][796]);
        applyDeltas(network_weights_input, network_weights_output, threshold, deltaInputWeights, deltaOutputWeights, deltaThresholds, eta);
        //run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        printf("RunLucas says: %f\n", activated_neurons[1][796]);
        runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        printf("after %d      backprop step, neuron fires at %f, bias is %0.20f and weight %d is %.20f\n", j, activated_neurons[1][796], threshold[1][796], 0, network_weights_output[0][0]);
    }
    /*for (int j = 0; j < 4; j++){
        for (int i = 0; i < 100; i++)
            backpropStep(board, player, 796, 1, network_weights_input, network_weights_output, threshold, activated_neurons, legal_moves, deltaInputWeights, deltaOutputWeights, deltaThresholds);
        applyDeltas(network_weights_input, network_weights_output, threshold, deltaInputWeights, deltaOutputWeights, deltaThresholds, eta);
        //run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        printf("RunLucas says: %f\n", activated_neurons[1][796]);
        runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
        printf("after 100    backprop step, neuron fires at %f, bias is %0.20f and weight %d is %.20f\n", activated_neurons[1][796], threshold[1][796], weightToCheck, network_weights_output[796][weightToCheck]);
    }
    writeToFile("testNetwork", network_weights_input, network_weights_output, threshold);*/
    return 0;
}