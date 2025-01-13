#include "agent.h"

double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons];    //array for the summed up gradients for weights between input and hidden layer and hidden and output layer (respectively), needs to be multiplied by eta and then added to the actual network
double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons];  //order is [intoNeuronNumber][outOfNeuronNumber]
double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons];            //same, but for biases, [layerNumber][neuronNumber], layer 0 is first hidden layer

double sigderiv(double x){
    //derivative of sigmoid for gradient calculations
    return exp(-x)/((exp(x)+1)*(exp(x)+1));
}

double invsig(double x){
    //inverse of sigmoid function to avoid having to calculate big sums again when trying to get to the inside of the sigmoid of a neuron to derive it (luckily sigmoid has an inverse)
    return log(x/(1-x));
}

//TODO get legalMoves
double* getExpected(char board[8][8], int player, int move, int outcome){
    //get desired values of output neurons based on move, board state and outcome of game (will need legalMoves function)
    double* expected = malloc(sizeof(double)*Number_of_Output_Neurons);
    int sumLegalMoves = 0;
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        if (legalMoves[i]){
            if (outcome == 1){
                //won
                if (i == move){
                    expected[i]=1; //we want the move we took since we won with it
                }else{
                    expected[i]=0;  //we don't want the moves we didn't take since we won with the one we took
                }
            }else if (outcome == -1){
                //lost
                if (i == move){
                    expected[i]=0; //we don't want the move we took since we lost with it
                }else{
                    expected[i]=1/(sumLegalMoves-1); //we want all other moves to be equally likely since we want one of them but can't say which one
                }
            }else if (outcome == 0){
                //drew, we take half of both the win and loss calculations
                if(i == move){
                    expected[i]=0.5;
                }else{
                    expected[i]=0.5/(sumLegalMoves-1);
                }
            }
        }else{
            expected[i]=0; //we want no illegal moves
        }
    }
}

void backpropStep(char board[8][8], int player, int move, int outcome) {
    //calculates the gradients of cost function with respect to the weights and biases for one move in one specified board state considering whether or not the agent won (outcome is 1 if won, -1 if lost, 0 if draw)
    //sums them up and writes them into deltaWeights and deltaThresholds, they will need to be eta'd and added outside of this function
    double expected[Number_of_Output_Neurons] = getExpected(board, move, player, outcome); //desired outputs to calculate cost function (e_n in scribbles)
    double activatedNeurons[Number_of_Layer-1][Number_of_Output_Neurons]; //actual values of the neurons in calculations (a_i_n in scribbles) TODO is this global?
    int inputNeurons[Number_of_Input_Neurons]; //TODO get these from Lucas or calulate them from board and player
    //TODO WIR MÜSSEN HIER WOHL NOCHMAL DAS NETZWERK LAUFEN LASSEN; ALLE SACHEN SPEICHERN WÄRE VERRÜCKT

    //loop for output layer going into hidden layer
    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        double preFactor = (activatedNeurons[Number_of_Layer-2][i]-expected[i])*(sigderiv(invsig(activatedNeurons[Number_of_Layer-2][i]))); //factor that is the same for all the gradients
        expected[i] = 0; //reuse expectation array to save expectations for previous layer (value is used for the only time in the line above)
        deltaThresholds[Number_of_Layer-2][i] +=preFactor; //gradient for bias, (since inner derivative of sigmoid for b is 1, this is just preFactor)
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            deltaOutputWeights[i][j] += preFactor*activatedNeurons[Number_of_Layer-3][j];
            expected[j] += preFactor*network_weights_output[i][j];   //reusing expected for gradients of next neurons here, which segFaults if hiddenNeurons > outputNeurons, but works for us
        }
    }
    //now expected is overwritten with the gradients that we want the neurons in the hidden layer to have, we will use those in the next step
    //
    //here we would loop over the connections between hidden layers, but we only have one, so that isn't needed
    //
    //now we need to look at the first (in this case only) hidden layer without doing expected (because we can't gradient descent the input)
    for (int i = 0; i < Number_of_Hidden_Neurons; i++) {
        double preFactor = expected[i]*(sigderiv(invsig(activatedNeurons[0][i]))); //factor that is the same for all the gradients, expected is now used differently, because it is no longer the expected value, but now the expected change
        deltaThresholds[0][i] += preFactor; //gradient for bias
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            deltaWeights[i][j] += preFactor*inputNeurons[j];
        }
    }
    //FREE THEM MALLOCS
}