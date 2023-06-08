#ifndef ARMV8_47_UTILS_H
#define ARMV8_47_UTILS_H

#include <stdint.h>
#include <stdbool.h>

// Function Declaration
uint32_t extractBits(uint32_t instruction, int startIndex, int endIndex);
uint32_t getBit(uint32_t bits, int bitIndex);
bool getMSB(uint32_t instruction);
const char* bitsToString(uint32_t value, int length);
bool matchesPattern(uint32_t bits, const char pattern[]);
#endif //ARMV8_47_UTILS_H
