#include "utils.h"

// Returns bits from startIndex to endIndex of the given instruction
uint32_t extractBits(uint32_t instruction, int startIndex, int endIndex) {
    int numBits = endIndex - startIndex + 1;

    // Determine the start and end bit positions for little-endian ordering
    int startBitPos = startIndex % 32;
    int endBitPos = endIndex % 32;

    // Extract the bits based on little-endian bit ordering
    uint32_t extractedBits = (instruction >> startBitPos) & ((1 << numBits) - 1);

    // Reverse the bit order for little-endian bit ordering
    uint32_t reversedBits = 0;
    for (int i = 0; i < numBits; i++) {
        reversedBits |= ((extractedBits >> i) & 1) << (numBits - 1 - i);
    }

    return reversedBits;
}

// Returns the bit at the bitIndex of the given instruction
uint32_t getBit(uint32_t bits, int bitIndex) {
    return extractBits(bits, bitIndex, bitIndex);
}

// Return the Most Significant Bit (MSB) of an instruction
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
    for (int i = 0; i < len; i++) {
        if (pattern[i] != 'X' && bitString[i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

char *initialise_array(unsigned int size)
{
    char *array = (char *)calloc(size, sizeof(char));
    if (!array)
    {
        perror("Unable to allocate memory for array.\n");
        exit(EXIT_FAILURE);
    }
    return array;
}

char **initialise_2d_array(unsigned int rows, unsigned int cols)
{
    char **array = calloc(rows, sizeof(char *));
    if (!array)
    {
        perror("Unable to allocate memory for pointer array.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++)
    {
        array[i] = calloc(cols, sizeof(char));
        if (!array[i])
        {
            perror("Unable to allocate memory for array.\n");
            exit(EXIT_FAILURE);
        }
    }

    return array;
}

void destroy_2d_array(char **arr, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(arr[i]);
    }
    free(arr);
}