#ifndef ARMV8_47_UTILS_H
#define ARMV8_47_UTILS_H

#include <stdint.h>

// Function Declaration
extern uint32_t extractBits(uint32_t instr, int startIndex, int endIndex);
extern bool getMSB(uint32_t instr);
extern uint32_t getBit(uint32_t bits, int bitIndex);
extern const char* bitsToString(uint32_t value, int length);
extern bool matchesPattern(uint32_t bits, const char pattern[]);
#endif //ARMV8_47_UTILS_H
