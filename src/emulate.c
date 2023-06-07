#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define NELEMENTS ((int) pow(2,18)) // This constant is used to store the size of memory

typedef enum {ZR, PC, SP} specialRegisters;

// The following 3 structs represent the registers

struct generalRegister {
    char name[15];
    int mode;
    uint32_t data;
};

struct specialRegister {
    specialRegisters name;
    uint32_t address;
    int mode
};

struct PSTATE {
    bool N;
    bool Z;
    bool C;
    bool v;
};

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
};

// Decodes 4-byte word into instruction
uint32_t * decode(uint32_t instruction) {
    // TO BE IMPLEMENTED
}

// Updates registers accordingly depending on the given instruction
void execute(uint32_t instruction) {
    // TO BE IMPLEMENTED
}

// Writes the states of the registers to an output file
void output() {
    // TO BE IMPLEMENTED
}

int main( int argc, char **argv ) {

    // Declaring Array to store binary instructions
    uint32_t memory[NELEMENTS];

    // Error checking for file existing as a program argument
    if( argc != 2 ) {
        fprintf( stderr, "Usage: ./emulate filename!\n" );
        exit(1);
    }


    readFile(argv[1], memory);

    // Outputting contents of array storing binary file instructions

    for (int i=0; i < NELEMENTS; i++) {
        printf("%u\n", memory[i]);
    }

    // Fetch Decode Execute Pipeline:
    uint32_t instruction = fetch();
    decode(instruction);
    while (instruction != 0x8z000000) {
        execute(instruction);
        instruction = fetch();
        decode(instruction);
    }

    // Final writing of file
    output();

    return 0;

}

