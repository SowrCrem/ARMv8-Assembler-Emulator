#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

typedef struct {
    uint32_t bits1;
    uint32_t bits2;
} bitsPair;

uint32_t extractBits(uint32_t instruction, int startIndex, int endIndex) {
    // Calculate the number of bits to extract
    int numBits = endIndex - startIndex + 1;
    uint32_t mask = (1 << numBits) - 1;
    mask <<= startIndex;
    uint32_t extractedBits = (instruction & mask) >> startIndex;
    return extractedBits;
}

uint32_t getBit(uint32_t bits, int bitIndex) {
    return extractBits(bits, bitIndex, bitIndex);
}

bool msb(uint32_t instr) {
    return getBit(instr, 31);
}

const char* bitsToString(uint32_t bits) {
    char* bitsString = "1111";
    return bitsString;
}

bool matchesPattern(uint32_t bits, const char pattern[]) {
    const char* bitStringPointer = bitsToString(bits);
    char bitString[] = "";
    strcpy(bitString, bitStringPointer);
    printf("%s", bitString);
    // int len = strlen(pattern);
    return true;
}

int main() {
    matchesPattern(15, "1111");
}
