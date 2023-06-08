#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "utils.h"
#include "registerAndMemory.h"
#include "singleDataTransferInstruction.h"
#include "dataProcessingImmediates.h"
#include "dataProcessingRegister.h"
#include "branch.h"

#define NO_ELEMENTS ((int) pow(2,18)) // This constant is used to store the size of memory
#define TERMINATE_INSTRUCTION 0x8a000000    // AND x0 x0 x0
#define NO_OP_INSTRUCTION 0xd503203f

enum instructionType {
    NOP=-1,
    UNRECOGNISED=0,
    DP_IMMEDIATE=1,
    DP_REGISTER=2,
    SINGLE_DATA_TRANSFER=3,
    LOAD_LITERAL=4,
    BRANCH=5
};


// Puts the instructions stored in the binary file into an array
void readFile(char* file, uint32_t data[]) {
    FILE *fp = fopen( file, "rb" ); // Open the file
    if (fp == NULL) { // Error check for opening file
        fprintf( stderr, "cat: can’t open %s\n", file );
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // Jump to the end of the file
    long fileLen = ftell(fp); // Get the current byte offset in the file
    rewind(fp); // Jump back to the beginning of the file
    fread(data, sizeof(uint32_t), fileLen/4, fp); // Read binary file
    if (ferror(fp)) {
        fprintf( stderr, "Error occurred reading from output.txt\n" );
        exit(1);
    }
    fclose(fp); // Close the Binary file
}

// Retrieves next 4-byte instruction from memory
uint32_t fetch(const uint32_t memory[]) {
    // Read PC register
    uint32_t programCounter = readPC();
    // Dereference the pointer to access the pointed instruction in memory
    uint32_t instruction = *(uint32_t*)(memory + programCounter);
    return instruction;
};

// Decodes 4-byte word into instruction by returning the instruction type
enum instructionType decode(uint32_t instruction) {
    if (instruction == NO_OP_INSTRUCTION) {
        return NOP;
    }
    uint32_t op0 = extractBits(instruction, 25, 28);
    if (matchesPattern(op0, "100X")) {
        return DP_IMMEDIATE;
    }
    if (matchesPattern(op0, "X101")) {
        return DP_REGISTER;
    }
    if (matchesPattern(op0, "X1X0")) {
        if (getMSB(instruction)) {
            return SINGLE_DATA_TRANSFER;
        }
        return LOAD_LITERAL;
    }
    if (matchesPattern(op0, "101X")) {
        return BRANCH;
    }
        return UNRECOGNISED;
}

// Updates registers accordingly depending on the given instruction
void execute(uint32_t instruction) {
    int instructionType = decode(instruction);
    switch (instructionType) {
        case DP_IMMEDIATE:
            dataProcessingImmediateInstruction(instruction);
            break;
        case DP_REGISTER:
            dataProcessingRegister(instruction);
            break;
        case SINGLE_DATA_TRANSFER:
            singleDataTransfer(instruction);
            break;
        case LOAD_LITERAL:
            // loadLiteral.c(instruction);
            break;
        case BRANCH:
            branch(instruction);
            break;
        default:    // nop - No Operation - skips operation
            break;
    }

    // Increment the PC after executing instruction
    writePC32(readPC() + 4, 32);

}

// Writes the states of the registers to an output file
void output() {
    printf("Registers:\n");
    for (int i = 0; i < 30; ++i) {
        printf("X%02d = %lx\n", i, readGeneral(i, 64));
    }

    printf("PC = %lx\n", readPC());
    bool vars[] = {readN(), readZ(), readC(), readV()};
    char letters[] = {'N', 'Z', 'C', 'V'};
    int size =  sizeof(vars) / sizeof(vars[0]);     // to calculate number of elements in array

    printf("PSTATE : ");
    for (int i = 0; i < size; i++) {
        if (vars[i]) {
            printf("%c", letters[i]);
        } else {
            printf("-");
        }
    }
    printf("\n");

    printf("Non-zero memory:\n");
    for (int i = 0; i < NO_ELEMENTS; ++i) {
        if (readMemory(i) != 0) {
            printf("%#x: %#x", i, readMemory(i));
        }
    }
}

int main( int argc, char **argv ) {

    construct();
    // Error checking for file existing as a program argument
    if (argc != 2) {
        fprintf( stderr, "Usage: ./emulate filename!\n" );
        exit(1);
    }

    readFile(argv[1], getMemory());

    // Fetch Decode Execute Pipeline:
    uint32_t instruction = fetch(getMemory());
    while (instruction != TERMINATE_INSTRUCTION) {
        // execute(instruction);
        instruction = fetch(getMemory());
    }

    // Final writing of file
    output();

    // Free memory after termination
    freeMemory();

    return 0;
}