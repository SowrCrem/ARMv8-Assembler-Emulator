#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "register.h"
#include "utils.h"

#define NO_ELEMENTS ((int) pow(2,18)) // This constant is used to store the size of memory
#define TERMINATE_INSTRUCTION 0x8a000000

// Puts the instructions stored in the binary file into an array
void readFile(char* file, uint32_t data[]) {
    // Open the file
    FILE *fp = fopen( file, "rb" );

    // Error check for opening file
    if( fp == NULL ) {
        fprintf( stderr, "cat: can’t open %s\n", file );
        exit(1);
    }

    // Jump to the end of the file
    fseek(fp, 0, SEEK_END);

    // Get the current byte offset in the file
    long fileLen = ftell(fp);

    // Jump back to the beginning of the file
    rewind(fp);

    // Reading binary file
    fread( data, sizeof(uint32_t), fileLen/4, fp);

    if( ferror(fp) ) {
        fprintf( stderr, "Error occurred reading from output.txt\n" );
        exit(1);
    }

    // Close the Binary file
    fclose(fp);
}

// Retrieves next 4-byte instruction from memory
uint32_t fetch() {
    // TO BE IMPLEMENTED
    return 0;
};

// Decodes 4-byte word into instruction by returning a number from 1 to 6 specifying the instruction type
int decode(uint32_t instruction) {
    uint32_t op0 = extractBits(instruction, 25, 28);

}

// Updates registers accordingly depending on the given instruction
void execute(uint32_t instruction) {
    int instructionType = decode(instruction);
}

// Writes the states of the registers to an output file
void output() {
    // TO BE IMPLEMENTED
}

int main( int argc, char **argv ) {

    // Declaring Array to store binary instructions
    uint32_t memory[NO_ELEMENTS];

    // Error checking for file existing as a program argument
    if( argc != 2 ) {
        fprintf( stderr, "Usage: ./emulate filename!\n" );
        exit(1);
    }


    readFile(argv[1], memory);

    // Outputting contents of array storing binary file instructions
//
//    for (int i=0; i < NO_ELEMENTS; i++) {
//        printf("%u\n", memory[i]);
//    }

    // Fetch Decode Execute Pipeline:
    uint32_t instruction = fetch();
    while (instruction != TERMINATE_INSTRUCTION) {
        execute(instruction);
        instruction = fetch();
    }

    // Final writing of file
    output();

    return 0;

}