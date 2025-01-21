#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>

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
            threshold[n][i] = 0.0;
        }
    }
}

void softmax(double *output, double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons], bool legal_moves[Number_of_Output_Neurons], double amplifier) {
    //performs softmax over legal moves in output layer to create a probability distribution
    //illegal moves are set to 0 likelyhood in output
    //values are multiplied by amplifier before being plugged into softmax to allow for more or less tendency to pick higher value moves
    
    double divisor = 0;

    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        if (legal_moves[i]) {
            divisor += exp(activated_neurons[Number_of_Layer-2][i]*amplifier);
        }
    }

    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        if (legal_moves[i]) {
            output[i] = exp(activated_neurons[Number_of_Layer-2][i]*amplifier) / divisor;
        } else {
            output[i] = 0;
        }
    }
}

int determineMove(double distribution[Number_of_Output_Neurons]){
    //randomly determines which move to use
    //expects softmaxed distibution
    //needs srand to have run
    double randNum = (double)rand() / RAND_MAX; //randomly generated number to choose move
    double sum = 0; //where in the sum of skipped possible moves we currently are
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        sum += distribution[i]; // adding current probability to accumulated probability
        if (randNum <= sum){
            return i; //return index if randNum is less than accumulated value after adding
        }
    }
    int out = 0;
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        if (distribution[i]!=0){
            out = i;
        }
    }
    return out; //returning last legal move if we break out of upper loop, assuming that happens if the sum is slightly less than one due to float problems (and we roll a random number really close to 1) 
}

void readFromFile(char *filename, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons]){
    //reads all the weights and biases from filename into the arrays in order of declaration
    FILE *f = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, f); //skip descriptor line
    //for input layer
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            getline(&line, &len, f); //read line
            line[strlen(line)-1] = '\0'; //remove newline
            network_weights_input[i][j] = atof(line);
        }
    }
    getline(&line, &len, f); //skip descriptor line
    //for output layer
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            getline(&line, &len, f); //read line
            line[strlen(line)-1] = '\0'; //remove newline
            network_weights_output[i][j] = atof(line);
        }
    }
    getline(&line, &len, f); //skip descriptor line
    //for thresholds
    for (int i = 0; i < Number_of_Layer-1; i++){
        for (int j = 0; j < Number_of_Output_Neurons; j++){
            getline(&line, &len, f); //read line
            line[strlen(line)-1] = '\0'; //remove newline
            threshold[i][j] = atof(line);
        }
    }
    free(line);
    fclose(f);
}

void writeToFile(char *filename, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons]){
    //writes all the weights and biases into filename in order of declaration
    FILE *f = fopen(filename, "w");
    fprintf(f, "Input Weights\n");
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        for (int j = 0; j < Number_of_Input_Neurons; j++){
            fprintf(f, "%.*e\n", DECIMAL_DIG, network_weights_input[i][j]);
        }
    }
    fprintf(f, "Output Weights\n");
    //for output layer
    for (int i = 0; i < Number_of_Output_Neurons; i++){
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            fprintf(f, "%.*e\n", DECIMAL_DIG, network_weights_output[i][j]);
        }
    }
    fprintf(f, "Thresholds\n");
    //for biases
    for (int i = 0; i < Number_of_Layer-1; i++){
        for (int j = 0; j < Number_of_Output_Neurons; j++){
            fprintf(f, "%.*e\n", DECIMAL_DIG, threshold[i][j]);
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
    //same as in the hidden
    for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
        sum += activated_neurons[layer-2][n] * network_weights_output[neuron_number][n];
    }
    sum += threshold[layer-1][neuron_number];
    
    //nosigNeurons[layer-1][neuron_number] = sum;

    activated_neurons[layer-1][neuron_number] = sigmoid(sum);
}

void hidden_layer(int layer, int neuron_number, double temp_board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]) {
    double sum = 0;
    //if the layer is 0, the current layer is the input-layer which contains the board state and player
    if (!layer) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                    sum += temp_board[x][y] * network_weights_input[neuron_number][(x * 8) + y]; //convert 2d Array into normel array with x*8, which means the row, plus y, the number of the column
            }
        }
        sum += player * network_weights_input[neuron_number][64]; //add to sum the player, who is next
    } else {
        //the "activated value" of each neuron is now the value to be multiplied by
        for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
            sum += activated_neurons[layer-1][n] * network_weights_input[neuron_number][n];
        }
        sum += threshold[layer][neuron_number];
    }

    //nosigNeurons[layer][neuron_number] = sum;

    activated_neurons[layer][neuron_number] = sigmoid(sum); //saving the return-value of the sigmoid function into the activated_neurons array
}

void input_layer(char board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]) {
    double temp_board[8][8];
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            temp_board[x][y] = (double)board[x][y]/100; //convert chars into double and squash down to reduce erratic behaviour
        }
    }
    //starting network stuff
    //to calc the "activated value" of each neuron, it calc out of the sum of the "actviated values" of the neurons one layer before
    for (int l = 0; l < Number_of_Layer-1; l++) {
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
    return exp(-x)/((exp(-x)+1)*(exp(-x)+1));
}

double* getExpected(char board[8][8], int player, int move, int outcome, bool legal_moves[Number_of_Output_Neurons]){
    //get desired values of output neurons based on move, board state and outcome of game (will need legalMoves function)
    bool _isHittingMove; //determineLegalMoves wants this, we don't need it
    double* expected = malloc(sizeof(double)*Number_of_Output_Neurons);
    int sumLegalMoves = determineLegalMoves(legal_moves, player, board, &_isHittingMove);
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
                    expected[i]=1.0/(sumLegalMoves-1); //we want all other moves to be equally likely since we want one of them but can't say which one
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

void getNeurons(double inputNeurons[Number_of_Input_Neurons], double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons], double nosigNeurons[Number_of_Layer-1][Number_of_Output_Neurons]){
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
    double inputNeurons[Number_of_Input_Neurons];
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            inputNeurons[(i*8)+j]=(double)board[i][j]/100; //saving chars from board into inputNeurons
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
    input_layer(board, player, network_weights_input, network_weights_output, threshold, activated_neurons); //rerunning network for current board state to get values into activated_neurons, we could also save these over the course of a game but that would take a lot of memory
    double inputNeurons[Number_of_Input_Neurons];
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            inputNeurons[(i*8)+j]=(double)board[i][j]/100; //saving chars from board into inputNeurons
            
        }
    }
    inputNeurons[Number_of_Input_Neurons-1] = player; // saving final input value that doesn't come from board array
    double hiddenNeuronDeriv[Number_of_Hidden_Neurons]; //array for derivative of hidden neurons as calculated from prev layer
    for (int i = 0; i < Number_of_Hidden_Neurons; i++){
        hiddenNeuronDeriv[i]=0;
    }
    getNeurons(inputNeurons, network_weights_input, network_weights_output, threshold, activated_neurons, nosigNeurons);
    //runNetworkFlo(board, player, network_weights_input, network_weights_output, threshold, activated_neurons);
    //START OF CALCULUS THINGS
    //loop for output layer going into hidden layer
    for (int i = 0; i < Number_of_Output_Neurons; i++) {
        double preFactor = (activated_neurons[Number_of_Layer-2][i]-expected[i])*(sigderiv(nosigNeurons[1][i])); //factor that is the same for all the gradients
        deltaThresholds[Number_of_Layer-2][i] += preFactor; //gradient for bias, (since inner derivative of sigmoid for b is 1, this is just preFactor)
        //loop for all the weights going into a_i
        for (int j = 0; j < Number_of_Hidden_Neurons; j++){
            deltaOutputWeights[i][j] += preFactor*activated_neurons[0][j];
            hiddenNeuronDeriv[j] += preFactor*network_weights_output[i][j];   //saving gradients of next neurons
        }
    }
    //now hiddenNeuronDeriv is filled with the gradients that we want the neurons in the hidden layer to have, we will use those in the next step
    //
    //here we would loop over the connections between hidden layers, but we only have one, so that isn't needed
    //
    //now we need to look at the first (in this case only) hidden layer without doing new neuron derivatives (because we can't gradient descent the input)
    for (int i = 0; i < Number_of_Hidden_Neurons; i++) {
        double preFactor = hiddenNeuronDeriv[i]*(sigderiv(nosigNeurons[0][i])); //factor that is the same for all the gradients, this time with hiddenNeuronDeriv instead of cost function
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