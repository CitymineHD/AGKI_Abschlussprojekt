#include <stdlib.h>
#include <stdio.h>

#include "antichess.h"
#include "agent.h"

int *memory = NULL;
int capazity = 0;
int written_elements = 0;

char *filename = ".replaymemory";

void memory_allocation() {
    capazity += 5;
    int *temp = malloc(capazity * sizeof(int));
    for (int i = 0; i < (capazity-5); i++) {
        temp[i] = memory[i];
    }
    free(memory);
    memory = temp;
}

void save_to_file() {
    FILE *f = fopen(filename, "a");

    for (int i = 0; i < written_elements; i++) {
        fprintf(f, "%d\n", memory[i]);
    }
}

void reset_memory() {
    save_to_file();
    capazity = 0;
    written_elements = 0;
    free(memory);
}

void add_to_ReplayMemory(int move) {
    if (written_elements >= capazity) {
        memory_allocation();
    }
    memory[written_elements] = move;
    written_elements++;
}

void save_ReplayMemory(int player) {
    if (written_elements >= capazity) {
        memory_allocation();
    }
    memory[written_elements] = player;
    written_elements++;
    reset_memory();
}