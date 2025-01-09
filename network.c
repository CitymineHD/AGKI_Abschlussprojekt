#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "agent.h"

#define Number_of_Input_Neurons 65
#define Number_of_Hidden_Neurons 2747
#define Number_of_Output_Neurons 4096

#define Number_of_Layer 5 //Input Layer + 3 Hidden Layer + Output Layer

double network_weights[Number_of_Layer-1][Number_of_Output_Neurons];
double threshold[Number_of_Layer-1][Number_of_Output_Neurons];

void initial_network_weights() {
    srand((unsigned int)time(NULL));
    for (int n = 0; n < (Number_of_Layer-1); n++) {
        for (int i = 0; i < Number_of_Output_Neurons; i++) {
            network_weights[n][i] = (float)rand() / RAND_MAX * 0.001;
            threshold[n][i] = (float)rand() / RAND_MAX;
        }
    }
}

double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

void output_layer(int layer, int neuron_number, double activated_neurons[Number_of_Layer][Number_of_Output_Neurons], int player) {
    double sum = 0;
    for (int n = 0; n < Number_of_Output_Neurons; n++) {
        sum += activated_neurons[layer-2][n] * network_weights[layer-2][n];
    }
    sum += threshold[layer-2][neuron_number];

    activated_neurons[layer-1][neuron_number] = sigmoid(sum);
}

void hidden_layer(int layer, int neuron_number, double activated_neurons[Number_of_Layer][Number_of_Output_Neurons], int temp_board[8][8], int player) {
    double sum = 0;
    if (!layer) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                    sum += temp_board[x][y] * network_weights[layer][(x * 8) + y];
            }
        }
        sum += player * network_weights[layer][64];
    } else {
        for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
            sum += activated_neurons[layer-1][n] * network_weights[layer][n];
        }
    }
    sum += threshold[layer][neuron_number];

    activated_neurons[layer][neuron_number] = sigmoid(sum);
}

void input_layer(char board[8][8], int player, double *output) {
    int temp_board[8][8];
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            temp_board[x][y] = board[x][y];
        }
    }

    double activated_neurons[Number_of_Layer][Number_of_Output_Neurons];

    for (int l = 0; l < Number_of_Layer; l++) {
        if (l != (Number_of_Layer-1)) {
            for (int n = 0; n < Number_of_Hidden_Neurons; n++) {
                hidden_layer(l, n, activated_neurons, temp_board, player);
            }
        } else {
            for (int n = 0; n < Number_of_Output_Neurons; n++) {
                output_layer(Number_of_Layer, n, activated_neurons, player);
            }
        }
    }

    for (int n = 0; n < Number_of_Output_Neurons; n++) {
        output[n] = activated_neurons[Number_of_Layer-1][n];
    }
}