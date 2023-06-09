#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"

typedef struct {
    uint32_t bits1;
    uint32_t bits2;
} bitsPair;

// Returns bits from startIndex to endIndex of the given instruction
uint32_t extractBits(uint32_t instruction, int startIndex, int endIndex) {
    // Calculate the number of bits to extract
    int numBits = endIndex - startIndex + 1;
    uint32_t mask = (1 << numBits) - 1;
    mask <<= startIndex;
    uint32_t extractedBits = (instruction & mask) >> startIndex;
    return extractedBits;
}

// Returns bit at the bitIndex of the given instruction
uint32_t getBit(uint32_t bits, int bitIndex) {
    return extractBits(bits, bitIndex, bitIndex);
}

// Returns a pair of instructions, split from 0-splitIndex (Inclusive) and the rest for the second half
bitsPair splitBits(uint32_t bits, int splitIndex) {
    //TODO
    bitsPair splitted;
    splitted.bits1 = bits;
    splitted.bits2 = bits;
    return splitted;
}


bool getMSB(uint32_t instruction) {
    return getBit(instruction, 31);
}

// Returns the binary representation, as a string, of a value.
const char *bitsToString(uint32_t value, int length) {
    char *binaryString = (char *) malloc((length + 1) * sizeof(char));  // Allocate memory for the binary string
    if (binaryString == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }
    binaryString[length] = '\0';  // Null-terminate the string
    for (int i = length - 1; i >= 0; i--) {
        binaryString[i] = (value & 1) ? '1' : '0';  // Extract the least significant bit
        value >>= 1;  // Shift the value to the right
    }
    return binaryString;
}

// Determines whether the given bits match a given string pattern, including dontcares ("X"s)
bool matchesPattern(uint32_t bits, const char pattern[]) {
    int len = (int) strlen(pattern);
    const char *bitString = bitsToString(bits, len);
    printf("%s", bitString);
    for (int i = 0; i < len; i++) {
        if (pattern[i] != 'X' && bitString[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

// For Testing Purposes
//int main() {
//    bool match = matchesPattern(10, "10X0");
//    printf("\n%d", match);
//}