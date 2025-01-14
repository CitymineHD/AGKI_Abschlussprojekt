#include <stdio.h>

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

    double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons];    //array for the summed up gradients for weights between input and hidden layer and hidden and output layer (respectively), needs to be multiplied by eta and then added to the actual network
    double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons];  //order is [intoNeuronNumber][outOfNeuronNumber]
    double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons];            //same, but for biases, [layerNumber][neuronNumber], layer 0 is first hidden layer
    initializeDeltas(deltaInputWeights, deltaOutputWeights, deltaThresholds);
    initial_network_weights(network_weights_input, network_weights_output, threshold);
    run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    determineLegalMoves(legal_moves, player, board);
    printf("this is test. pls take a moment to pray this shit works.\n neuron for e2e4: %f, is it legal? %d\n", activated_neurons[1][796], legal_moves[796]);
    backpropStep(board, player, 796, 1, network_weights_input, network_weights_output, threshold, activated_neurons, legal_moves, deltaInputWeights, deltaOutputWeights, deltaThresholds);
    applyDeltas(network_weights_input, network_weights_output, threshold, deltaInputWeights, deltaOutputWeights, deltaThresholds, 0.0001);
    run_network(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    printf("after single backprop step, neuron fires at %f (pls be more than before plspls)\n", activated_neurons[1][796]);

    return 0;
}