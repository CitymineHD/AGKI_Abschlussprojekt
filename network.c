#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#include "agent.h"

#define Number_of_Input_Neurons 65
#define Number_of_Hidden_Neurons 2747
#define Number_of_Output_Neurons 4096

#define Number_of_Layer 3 //Input Layer + Hidden Layer + Output Layer

double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons];
double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons];
double threshold[Number_of_Layer-1][Number_of_Output_Neurons];

double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons];

bool legal_moves[4096]; //TODO: Implement Array from Philipp

//initial network weights  and thresholds randomly
void initial_network_weights() {
    srand((unsigned int)time(NULL)); //:-) the easiest way
    for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
            for (int x = 0; x < Number_of_Input_Neurons; x++) {
                network_weights_input[n][x] = (float)rand() / RAND_MAX * 0.001;
            }
    }
    //initial weights
    for (int n = 0; n < Number_of_Output_Neurons; n++) {
        for (int i = 0; i < Number_of_Output_Neurons; i++) {
            network_weights_output[n][i] = (float)rand() / RAND_MAX * 0.001;
        }
    }
    //inital thresholds
    for (int n = 0; n < (Number_of_Layer-1); n++) {
        for (int i = 0; i < Number_of_Output_Neurons; i++) {
            threshold[n][i] = 0.5;
        }
    }
}

//sigmoid function
double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

void output_layer(int layer, int neuron_number, int player) {
    double sum = 0;
    for (int n = 0; n < Number_of_Output_Neurons; n++) {
        sum += activated_neurons[layer-2][n] * network_weights_output[neuron_number][n];
    }
    sum += threshold[layer-2][neuron_number];

    activated_neurons[layer-1][neuron_number] = sigmoid(sum);
}

void hidden_layer(int layer, int neuron_number, int temp_board[8][8], int player) {
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
    }
    sum += threshold[layer][neuron_number];

    activated_neurons[layer][neuron_number] = sigmoid(sum);
}

void softmax(double *output) {
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

void input_layer(char board[8][8], int player, double *output) {
    int temp_board[8][8];
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            temp_board[x][y] = board[x][y];
        }
    }

    for (int l = 0; l < (Number_of_Layer-1); l++) {
        if (l != (Number_of_Layer-1)) {
            for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
                hidden_layer(l, n, temp_board, player);
            }
        } else {
            for (int n = 0; n < Number_of_Output_Neurons; n++) {
                output_layer((Number_of_Layer-1), n, player);
            }
        }
    }
    softmax(output);
}