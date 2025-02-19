#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#include "agent.h"
#include "antichess.h"

//double nosigNeurons[Number_of_Layer-1][Number_of_Output_Neurons];

//initial network weights  and thresholds randomly
void initial_network_weights(double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons]) {
    srand((unsigned int)time(NULL)); //:-) the easiest way
    for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
            for (int x = 0; x < Number_of_Input_Neurons; x++) {
                network_weights_input[n][x] = (float)rand() / RAND_MAX * 0.001;
            }
    }
    //initial weights
    for (int n = 0; n < Number_of_Output_Neurons; n++) {
        for (int i = 0; i < Number_of_Hidden_Neurons; i++) {
            network_weights_output[n][i] = (float)rand() / RAND_MAX * 0.001;
        }
    }
    //inital thresholds
    for (int n = 0; n < (Number_of_Layer-1); n++) {
        for (int i = 0; i < Number_of_Output_Neurons; i++) {
            threshold[n][i] = 0.005;
        }
    }
}

void softmax(double *output, double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons], bool legal_moves[Number_of_Output_Neurons]) {
    double divisor = 0;

    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        if (legal_moves[i]) {
            divisor += activated_neurons[Number_of_Layer-2][i];
        }
    }

    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        if (legal_moves[i]) {
            output[i] = activated_neurons[Number_of_Layer-2][i] / divisor;
        } else {
            output[i] = 0;
        }
    }
}

void readFromFile(char *filename){
    //reads all the weights and biases from filename into the arrays in order of declaration
    FILE *f = fopen(filename, "r");

    fclose(f);
}

void writeToFile(char *filename, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons]){
    //writes all the weights and biases into filename in order of declaration
    FILE *f = fopen(filename, "w");
    fprintf(f, "Number of Input Neuron, Weight\n");
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            fprintf(f, "%d: %f\n", i, network_weights_input[i][j]);
        }
    }
    fprintf(f, "Number of Output Neuron, Weight\n");
    //for output layer
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            fprintf(f, "%d: %f\n", i, network_weights_output[i][j]);
        }
    }
    fprintf(f, "Number of Layer/Neuron, Threshold\n");
    //for biases
    for (int i = 0; i < Number_of_Layer-1; i++){
        for (int j = 0; j < Number_of_Output_Neurons; j++){
            fprintf(f, "%d/%d: %f\n", i, j, threshold[i][j]);
        }
    }

    fclose(f);
}

//sigmoid function
double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

void output_layer(int layer, int neuron_number, int player, double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]) {
    double sum = 0;
    for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
        sum += activated_neurons[layer-2][n] * network_weights_output[neuron_number][n];
    }
    sum += threshold[layer-1][neuron_number];
    
    //nosigNeurons[layer-1][neuron_number] = sum;

    activated_neurons[layer-1][neuron_number] = sigmoid(sum);
}

void hidden_layer(int layer, int neuron_number, int temp_board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]) {
    double sum = 0;
    if (!layer) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                    sum += temp_board[x][y] * network_weights_input[neuron_number][(x * 8) + y];
            }
        }
        sum += player * network_weights_input[neuron_number][64];
    } else {
        for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
            sum += activated_neurons[layer-1][n] * network_weights_input[neuron_number][n];
        }
        sum += threshold[layer][neuron_number];
    }

    //nosigNeurons[layer][neuron_number] = sum;

    activated_neurons[layer][neuron_number] = sigmoid(sum);
}

void input_layer(char board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]) {
    int temp_board[8][8];
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            temp_board[x][y] = board[x][y];
        }
    }

    for (int l = 0; l < 2; l++) {
        if (l != 1) {
            for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
                hidden_layer(l, n, temp_board, player, network_weights_input, threshold, activated_neurons);
            }
        } else {
            for (int n = 0; n < Number_of_Output_Neurons; n++) {
                output_layer(2, n, player, network_weights_output, threshold, activated_neurons);
            }
        }
    }
}

//backprop stuff (in same file now to use global variables easily)

double sigderiv(double x){
    //derivative of sigmoid for gradient calculations
    return exp(-x)/((exp(x)+1)*(exp(x)+1));
}

double invsig(double x){
    //inverse of sigmoid function to avoid having to calculate big sums again when trying to get to the inside of the sigmoid of a neuron to derive it (luckily sigmoid has an inverse)
    return log(x/(1-x));
}


double* getExpected(char board[8][8], int player, int move, int outcome, bool legal_moves[Number_of_Output_Neurons]){
    //get desired values of output neurons based on move, board state and outcome of game (will need legalMoves function)
    double* expected = malloc(sizeof(double)*Number_of_Output_Neurons);
    int sumLegalMoves = determineLegalMoves(legal_moves, player, board);
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        if (legal_moves[i]){
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
    return expected;
}

void getNeurons(int inputNeurons[Number_of_Input_Neurons], double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons], double nosigNeurons[Number_of_Layer-1][Number_of_Output_Neurons]){
    //summing up all the stuffs again for the neurons
    //for hidden neurons
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        double sum = 0;
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            sum += network_weights_input[i][j]*inputNeurons[j];
        }
        sum += threshold[0][i];
        nosigNeurons[0][i] = sum;
        //activated_neurons[0][i] = sigmoid(sum);
    }
    //for output neurons
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        double sum = 0;
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            sum += network_weights_output[i][j]*activated_neurons[0][j];
        }
        sum += threshold[1][i];
        nosigNeurons[1][i] = sum;
        //activated_neurons[1][i] = sigmoid(sum);
    }
}

void runNetworkFlo(char board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]){
    //different runNetwork for debugging
    int inputNeurons[Number_of_Input_Neurons];
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
           inputNeurons[(i*8)+j] =  board[i][j]; //saving chars from board into inputNeurons
        }
    }
    inputNeurons[Number_of_Input_Neurons-1] = player; // saving final input value that doesn't come from board array
    //for hidden neurons
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        double sum = 0;
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            sum += network_weights_input[i][j]*inputNeurons[j];
        }
        sum += threshold[0][i];
        activated_neurons[0][i] = sigmoid(sum);
    }
    //for output neurons
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        double sum = 0;
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            sum += network_weights_output[i][j]*activated_neurons[0][j];
        }
        sum += threshold[1][i];
        activated_neurons[1][i] = sigmoid(sum);
    }
}

void backpropStep(char board[8][8], int player, int move, int outcome, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons], bool legal_moves[Number_of_Output_Neurons], double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons]) {
    //calculates the gradients of cost function with respect to the weights and biases for one move in one specified board state considering whether or not the agent won (outcome is 1 if won, -1 if lost, 0 if draw)
    //sums them up and writes them into deltaWeights and deltaThresholds, they will need to be eta'd and added outside of this function
    double *expected = getExpected(board, player, move, outcome, legal_moves); //desired outputs to calculate cost function (e_n in scribbles) is a pointer, gets sizeofdouble**Number_of_Output_Neurons memory
    double nosigNeurons[Number_of_Layer-1][Number_of_Output_Neurons]; //saving neurons without sigmoid squish, because invsig got unfriendly when close to desired output
    //input_layer(board, player, network_weights_input, network_weights_output, threshold, activated_neurons); //rerunning network for current board state to get values into activated_neurons, we could also save these over the course of a game but that would take a lot of memory
    int inputNeurons[Number_of_Input_Neurons];
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            inputNeurons[(i*8)+j] = board[i][j]; //saving chars from board into inputNeurons
        }
    }
    inputNeurons[Number_of_Input_Neurons-1] = player; // saving final input value that doesn't come from board array
    getNeurons(inputNeurons, network_weights_input, network_weights_output, threshold, activated_neurons, nosigNeurons);
    runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    //START OF CALCULUS THINGS
    //loop for output layer going into hidden layer
    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        double preFactor = (activated_neurons[Number_of_Layer-2][i]-expected[i])*(sigderiv(nosigNeurons[1][i])); //factor that is the same for all the gradients
        expected[i] = 0; //reuse expectation array to save expectations for previous layer (value is used for the only time in the line above)
        deltaThresholds[Number_of_Layer-2][i] += preFactor; //gradient for bias, (since inner derivative of sigmoid for b is 1, this is just preFactor)
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            deltaOutputWeights[i][j] += preFactor*activated_neurons[0][j];
            expected[j] += preFactor*network_weights_output[i][j];   //reusing expected for gradients of next neurons here, which segFaults if hiddenNeurons > outputNeurons, but works for us
            //TODO THIS ADDS UP TO VERY BIG STUFFSIES, multiply with eta or divide by number of neurons generating the input
        }
    }
    //now expected is overwritten with the gradients that we want the neurons in the hidden layer to have, we will use those in the next step
    //
    //here we would loop over the connections between hidden layers, but we only have one, so that isn't needed
    //
    //now we need to look at the first (in this case only) hidden layer without doing expected (because we can't gradient descent the input)
    for (int i = 0; i < Number_of_Hidden_Neurons; i++) {
        double preFactor = expected[i]*(sigderiv(nosigNeurons[0][i])); //factor that is the same for all the gradients, expected is now used differently, because it is no longer the expected value, but now the expected change
        deltaThresholds[0][i] += preFactor; //gradient for bias
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            deltaInputWeights[i][j] += preFactor*inputNeurons[j];
        }
    }
    //FREE THEM MALLOCS
    free(expected);
}

void run_network(char board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]) {
    //run the network with the current board state and the player who's the next
    input_layer(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
}

void initializeDeltas(double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons]){
    //this is C, we need to initialize arrays to 0
    //for input layer
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            deltaInputWeights[i][j] = 0;
        }
    }
    //for output layer
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            deltaOutputWeights[i][j] = 0;
        }
    }
    //for biases
    for (int i = 0; i < Number_of_Layer-1; i++){
        for (int j = 0; j < Number_of_Output_Neurons; j++){
            deltaThresholds[i][j] = 0;
        }
    }
}

void applyDeltas(double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons], double eta){
    //applies deltas for all weights and biases to the actual values, multiplying them by the step width eta
    //then resets deltas to 0
    //subtracts because we are following negative gradient

    //for input layer
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            network_weights_input[i][j] -= eta*deltaInputWeights[i][j];
            deltaInputWeights[i][j] = 0;
        }
    }
    //for output layer
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            network_weights_output[i][j] -= eta*deltaOutputWeights[i][j];
            deltaOutputWeights[i][j] = 0;
        }
    }
    //for biases
    for (int i = 0; i < Number_of_Layer-1; i++){
        for (int j = 0; j < Number_of_Output_Neurons; j++){
            threshold[i][j] -= eta*deltaThresholds[i][j];
            deltaThresholds[i][j] = 0;
        }
    }
}