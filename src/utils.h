#ifndef ARMV8_47_UTILS_H
#define ARMV8_47_UTILS_H

#include <stdint.h>
#include <stdbool.h>

// Function Declaration
uint32_t extractBits(uint32_t instr, int startIndex, int endIndex);
bool getMSB(uint32_t instr);
uint32_t getBit(uint32_t bits, int bitIndex);
const char* bitsToString(uint32_t value, int length);
bool matchesPattern(uint32_t bits, const char pattern[]);
#endif //ARMV8_47_UTILS_H
