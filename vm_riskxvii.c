#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define INST_MEM_SIZE 1024
#define DATA_MEM_SIZE 1024


struct blob {
    char inst_mem[INST_MEM_SIZE];
    char data_mem[DATA_MEM_SIZE];
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return 1;
    }
    int index = 0;
    FILE *file = fopen(argv[1], "rb");
    struct blob b;
    while (!feof(file)) {
        uint32_t instruction;
        fread(&instruction, 4, 1, file);
        for (int i = 31; i >= 0; i--) {
            if (index < 1024) {
                b.inst_mem[index++] = (instruction >> i) & 1;
            } else if (index >= 1024) {
                b.data_mem[index - 1024] = (instruction >> i) & 1;
            }   
        }
    }
    printf("%d\n", index);
    for (int i = 0; i < 1024; i++) {
        printf("%x", b.inst_mem[i]);
    }
    fclose(file);
    return 0;
}