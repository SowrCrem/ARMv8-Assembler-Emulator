//
// Created by Vivian Lopez on 07/06/2023.
//

#ifndef ARMV8_47_UTILS_H
#define ARMV8_47_UTILS_H

#include <stdint.h>

// Function Declaration
uint32_t extractBits(uint32_t instr, int startIndex, int endIndex);
bool msb(uint32_t instr);

#endif //ARMV8_47_UTILS_H
