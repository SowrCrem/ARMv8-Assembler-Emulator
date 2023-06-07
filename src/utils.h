//
// Created by Vivian Lopez on 07/06/2023.
//

#ifndef ARMV8_47_UTILS_H
#define ARMV8_47_UTILS_H
#include <stdint.h>
#include <stdbool.h>

// Function Declaration
uint32_t extractBits(uint32_t instr, int start, int end);
bool msb(uint32_t instr);

#endif //ARMV8_47_UTILS_H
