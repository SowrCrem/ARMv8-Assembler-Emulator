#ifndef ARMV8_47_UTILS_H
#define ARMV8_47_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../global_var.h"

// Function Declaration
uint32_t extractBits(uint32_t instruction, int startIndex, int endIndex);

uint32_t getBit(uint32_t bits, int bitIndex);

bool getMSB(uint32_t instruction);

const char *bitsToString(uint32_t value, int length);

bool matchesPattern(uint32_t bits, const char pattern[]);

char *initialise_array(unsigned int size);
char **initialise_2d_array(unsigned int rows, unsigned int cols);
void destroy_2d_array(char **arr, int rows);

int no_lines_text_file(char *filename);
char **load_text_file(char *filename, int no_lines);

void write_binary_file(word_t *binary, char *filename, int no_instructions);

#endif //ARMV8_47_UTILS_H
