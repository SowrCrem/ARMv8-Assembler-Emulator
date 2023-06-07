//
// Created by Vivian Lopez on 07/06/2023.
//

#include <stdint.h>
#include "utils.h"

typedef struct {
    uint32_t bits1;
    uint32_t bits2;
} bitsPair;

uint32_t extractBits(uint32_t instruction, int start, int end) {
    // Calculate the number of bits to extract
    int numBits = end - start + 1;
    uint32_t mask = (1 << numBits) - 1;
    mask <<= start;
    uint32_t extractedBits = (instruction & mask) >> start;
    return extractedBits;
}

uint32_t getBit(uint32_t bits, int bit) {
    return extractBits(bits, bit, bit);
}

uint32_t

bool msb(uint32_t instr){
    return getBit(instr, 31);
}





