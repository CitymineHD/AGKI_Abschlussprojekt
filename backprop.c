#include "network.h"

double deltaWeights[][][];      //array for the summed up gradients for all the network weights, needs to be multiplied by eta and then added to the actual network
                                //order is [layerNumber][intoNeuronNumber][outOfNeuronNumber]
                                //this is extremely memory inefficient, we could store weights out of input layer and into output layer separately TODO
double deltaThresholds[][];     //same, but for biases, [layerNumber][neuronNumber]

double sigderiv(double x){
    //derivative of sigmoid for gradient calculations
    return exp(-x)/((exp(x)+1)*(exp(x)+1));
}

double invsig(double x){
    //inverse of sigmoid function to avoid having to calculate big sums again when trying to get to the inside of the sigmoid of a neuron to derive it (luckily sigmoid has an inverse)
    return log(x/(1-x));
}

double[Number_of_Output_Neurons] getExpected(char board[8][8], int move, int outcome){
    //get desired values of output neurons based on move, board state and outcome of game (will need legalMoves function)
    //TODO
}

void backpropStep(char board[8][8], int move, int outcome) {
    //calculates the gradients of cost function with respect to the weights and biases for one move in one specified board state considering whether or not the agent won
    //sums them up and writes them into deltaWeights and deltaThresholds, they will need to be eta'd and added outside of this function
    double expected[Number_of_Output_Neurons] = getExpected(board, move, outcome); //desired outputs to calculate cost function (e_n in scribbles)
    double activatedNeurons[Number_of_Layer][Number_of_Output_Neurons]; //actual values of the neurons in calculations (a_i_n in scribbles)
    //TODO WIR MÜSSEN HIER WOHL NOCHMAL DAS NETZWERK LAUFEN LASSEN; ALLE SACHEN SPEICHERN WÄRE VERRÜCKT

    //loop for output layer
    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        double preFactor = (activatedNeurons[Number_of_Layer-1][i]-expected[i])*(sigderiv(invsig(activatedNeurons[Number_of_Layer-1][i]))); //factor that is the same for all the gradients
        expected[i] = 0; //reuse expectation array to save expectations for previous layer (value is used for the only time in the line above)
        deltaThresholds[Number_of_Layer-2][i] +=preFactor; //gradient for bias
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            deltaWeights[Number_of_Layer-2][i][j] += preFactor*activatedNeurons[Number_of_Layer-2][j]; //TODO abgleichen mit Lucas
            expected[i] += preFactor*network_weights[Number_of_Layer-2][i][j];  //the reusing segFaults if hiddenNeurons > outputNeurons
                                                                                //TODO check if this works with new weight data structure
        }
    }
    //gradients for input layer are now added and gradients for prev layer neurons are saved to expected
    //now we loop over the hidden layers (except the last one, which has a different number of input weights) and do the same
    for (int k = 1; k < Number_of_Layer-2; k++){
        for (int i = 0; i < Number_of_Hidden_Neurons; i++) {
            double preFactor = (activatedNeurons[Number_of_Layer-1-k][i]-expected[i])*(sigderiv(invsig(activatedNeurons[Number_of_Layer-1-k][i]))); //factor that is the same for all the gradients
            expected[i] = 0; //reuse expectation array to save expectations for previous layer (value is used for the only time in the line above)
            deltaThresholds[Number_of_Layer-2][i] +=preFactor; //gradient for bias
            //loop for all the weights going into a_i
            for (int j = 0; j < Number_of_Hidden_Neurons; j++){
                deltaWeights[Number_of_Layer-2-k][i][j] += preFactor*activatedNeurons[Number_of_Layer-2-k][j]; //TODO abgleichen mit Lucas
                expected[i] += preFactor*network_weights[Number_of_Layer-2-k][i][j];  //the reusing segFaults if hiddenNeurons > outputNeurons
                                                                                    //TODO check if this works with new weight data structure
            }
        }
    }
    //now the connections between hidden layers are done and we only need to look at the first hidden layer without doing expected, because this is the last step
    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        double preFactor = (activatedNeurons[1][i]-expected[i])*(sigderiv(invsig(activatedNeurons[1][i]))); //factor that is the same for all the gradients
        expected[i] = 0; //reuse expectation array to save expectations for previous layer (value is used for the only time in the line above)
        deltaThresholds[0][i] +=preFactor; //gradient for bias
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            deltaWeights[0][i][j] += preFactor*activatedNeurons[0][j]; //TODO abgleichen mit Lucas
            expected[i] += preFactor*network_weights[Number_of_Layer-2][i][j];  //the reusing segFaults if hiddenNeurons > outputNeurons
                                                                                //TODO check if this works with new weight data structure
        }
    }
}