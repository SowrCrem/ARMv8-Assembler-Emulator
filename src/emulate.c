#include <math.h>
#include "global_var.h"
#include "utils/utils.h"
#include "utils/storage.h"
#include "instructions/dpImmediate.h"
#include "instructions/dpRegister.h"
#include "instructions/branch.h"
#include "instructions/dataTransfer.h"

static bool debug = true;

enum instructionType {
    NOP = -1,
    UNRECOGNISED = 0,
    DP_IMMEDIATE = 1,
    DP_REGISTER = 2,
    DATA_TRANSFER = 3,
    BRANCH = 4
};

void shiftArrayByFour(uint32_t array[], size_t size) {
    uint32_t *shiftedArray = (uint32_t *) malloc((size) * sizeof(uint32_t));

    // Initialise shifted array to 0
    for (size_t i = 0; i < size; i++) {
        shiftedArray[i] = 0;
    }

    // Copy shifted values from the original array
    for (size_t i = 0; i < size; i++) {
        shiftedArray[i * 4] = array[i];
    }

    for (size_t i = 0; i < size; i++) {
        array[i] = shiftedArray[i];
    }
}

// Puts the instructions stored in the binary file into an array
void readFile(char *file, uint32_t data[]) {
    FILE *fp = fopen(file, "rb"); // Open the file
    if (fp == NULL) { // Error check for opening file
        fprintf(stderr, "Can’t open %s\n", file);
        exit(1);
    }
    fseek(fp, 0, SEEK_END); // Jump to the end of the file
    long fileLen = ftell(fp); // Get the current byte offset in the file
    rewind(fp); // Jump back to the beginning of the file
    // size_t num_values = fileLen / (4 * sizeof(uint32_t));
    size_t num_values = fileLen / (sizeof(uint32_t));
    if (debug) {

    }
    fread(data, sizeof(uint32_t), num_values, fp); // Read binary file
    if (ferror(fp)) {
        fprintf(stderr, "Error occurred reading from output.txt\n");
        exit(1);
    }
    fclose(fp); // Close the Binary file

    shiftArrayByFour(data, 100);
}

// Retrieves next 4-byte instruction from memory
uint32_t fetch(const uint32_t memory[]) {
    // Read PC register
    uint32_t programCounter = readPC();
    // Dereference the pointer to access the pointed instruction in memory
    //uint32_t instruction = *(uint32_t *) (memory + programCounter);
    uint32_t instr = readMemory(programCounter);
    writePC64(readPC() + 4, 64); // Increment PC
    return instr;
}

// Decodes 4-byte word into instruction by returning the instruction type
enum instructionType decode(uint32_t instruction) {
    if (instruction == NO_OP_INSTRUCTION) {
        writePC64(readPC() - 4, 64);    // Decrement PC
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
        return DATA_TRANSFER;
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
            dataProcessingImmediate(instruction);
            break;
        case DP_REGISTER:
            dataProcessingRegister(instruction);
            break;
        case DATA_TRANSFER:
            dataTransfer(instruction);
            break;
        case BRANCH:
            branch(instruction);
            return;
        case UNRECOGNISED:
            printf("Unrecognised Instruction\n");
            break;
        default:    // nop - No Operation - skips operation
            break;
    }
}

// Writes the states of the registers to an output file
void output_stdout() {

    printf("Registers:\n");
    for (int i = 0; i < 30; ++i) {
        printf("X%02d = %lx\n", i, readGeneral(i, 64));
    }

    printf("PC = %lx\n", readPC());
    bool vars[] = {readN(), readZ(), readC(), readV()};
    char letters[] = {'N', 'Z', 'C', 'V'};
    int size = sizeof(vars) / sizeof(vars[0]);     // to calculate number of elements in array

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

void output_file(char *filename) {
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "Registers:\n");
    for (int i = 0; i < 31; ++i) {
        fprintf(fp, "X%02d    = %016lx\n", i, readGeneral(i, 64));
    }

    fprintf(fp, "PC     = %016lx\n", readPC());
    bool vars[] = {readN(), readZ(), readC(), readV()};
    char letters[] = {'N', 'Z', 'C', 'V'};
    int size = sizeof(vars) / sizeof(vars[0]);     // to calculate number of elements in array

    fprintf(fp, "PSTATE : ");
    for (int i = 0; i < size; i++) {
        if (vars[i]) {
            fprintf(fp, "%c", letters[i]);
        } else {
            fprintf(fp, "-");
        }
    }
    fprintf(fp, "\n");

    fprintf(fp, "Non-Zero Memory:\n");
    for (int i = 0; i < NO_ELEMENTS; ++i) {
        if (readMemory(i) != 0) {
            if (i == 0) {
                fprintf(fp, "0x00000000 : %x\n", readMemory(i));
            } else {
                fprintf(fp, "%#010x : %x\n", i, readMemory(i));
            }
        }
    }
    fclose(fp);
}

void printBinary(uint32_t num) {
    // Number of bits in an integer (assuming 32 bits)
    int numBits = sizeof(num) * 8;

    // Iterate over each bit from left to right
    for (int i = numBits - 1; i >= 0; i--) {
        // Check the value of the current bit
        int bit = (num >> i) & 1;

        // Print the bit
        printf("%d", bit);
    }

    //printf("\n");
}

void printArray(uint32_t arr[], int size) {
    printf("[ ");
    for (int i = 0; i < size; i++) {
        if (arr[i] != 0x0) {
            printf("[ %d index %d ]", arr[i], i);
        }
    }
    printf(" ]\n");
}

int main(int argc, char **argv) {

    construct();
    // Error checking for file existing as a program argument
    if (argc < 2) {
        fprintf(stderr, "Usage: ./emulate filename!\n");
        return EXIT_FAILURE;
    }

    readFile(argv[1], getMemory());
    if (debug) {
        printf("Non Zero Memory:\n");
        printArray(getMemory(), 50);
    }
    int count = 1;
    // Fetch Decode Execute Pipeline:
    uint32_t nextInstruction;
    while (nextInstruction != TERMINATE_INSTRUCTION) {
        if (debug) {
            printf("Instruction number: %d, instruction value: ", count);
            nextInstruction = fetch(getMemory());
        }
        if (decode(nextInstruction) == UNRECOGNISED) {
            break;
        }
        if (debug) {
            printBinary(nextInstruction);
            printf(", PC value: %lu \n", readPC());
        }
        execute(nextInstruction);
        if (debug) {
            count++;
        }
    }



    // Final writing of file
    if (argc == 2) {
        output_stdout();
    } else {
        output_file(argv[2]);
    }

    // Free memory after termination
    freeMemory();

    return EXIT_SUCCESS;
}