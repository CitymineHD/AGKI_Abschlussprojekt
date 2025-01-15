#include <stdbool.h>

#define Number_of_Input_Neurons 65
#define Number_of_Hidden_Neurons 2747
#define Number_of_Output_Neurons 4096

#define Number_of_Layer 3 //Input Layer + Hidden Layer + Output Layer

void initial_network_weights(double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons]);

void run_network(char board[8][8], int player, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons]);

void backpropStep(char board[8][8], int player, int move, int outcome, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double activated_neurons[Number_of_Layer-1][Number_of_Output_Neurons], bool legal_moves[Number_of_Output_Neurons], double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons]);

void applyDeltas(double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons], double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons], double eta);

void initializeDeltas(double deltaInputWeights[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double deltaOutputWeights[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double deltaThresholds[Number_of_Layer-1][Number_of_Output_Neurons]);

void writeToFile(char *filename, double network_weights_input[Number_of_Hidden_Neurons][Number_of_Input_Neurons], double network_weights_output[Number_of_Output_Neurons][Number_of_Hidden_Neurons], double threshold[Number_of_Layer-1][Number_of_Output_Neurons]);