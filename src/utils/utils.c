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

// FOR ASSEMBLER

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

int no_lines_text_file(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Text file cannot be opened.");
        exit(EXIT_FAILURE);
    }

    int no_lines = 0;
    int ch;
    char last_ch = '\n';
    while (EOF != (ch = fgetc(fp)))
    {
        if (ch == '\n' && last_ch != '\n')
        {
            ++no_lines;
        }
        last_ch = ch;
    }
    rewind(fp);

    return no_lines;
}

char **load_text_file(char *filename, int no_lines)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    char **text_file = initialise_2d_array(no_lines, MAX_LINE_LENGTH);

    // Filling the array with each line, stripping blank newlines in source file
    int i = 0;
    while (i < no_lines && fgets(text_file[i], MAX_LINE_LENGTH, fp))
    {
        if (text_file[i][0] != '\n')
        {
            text_file[i][strcspn(text_file[i], "\n")] = '\0';
            i++;
        }
    }

    fclose(fp);
    return text_file;
}