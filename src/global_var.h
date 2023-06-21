// Global constant, enum and type definitions

#ifndef ARMV8_47_GLOBAL_VAR_H
#define ARMV8_47_GLOBAL_VAR_H
#include <stdint.h>

#define DEBUG true // Enable debugging output

// emulate.c constants
#define NO_ELEMENTS ((int) pow(2,21))       // Used to store the size of memory
#define TERMINATE_INSTRUCTION 0x8a000000    // AND x0 x0 x0
#define NO_OP_INSTRUCTION 0xd503203f

#define WORD_SIZE 32 // Architecture word size

// Masks
#define MASK_1_LSB 0x1
#define MASK_2_LSB 0x3
#define MASK_4_LSB 0xF
#define MASK_5_LSB 0x1F
#define MASK_8_LSB 0xFF
#define MASK_12_LSB 0xFFF
#define MASK_24_LSB 0xFFFFFF
#define MASK_28_LSB 0xFFFFFFF
#define MASK_1_MSB 0x80000000

#define MAX_LINE_LENGTH 512 // Maximum length of an assembly line
#define MAX_NUMBER_OF_OPERANDS 5
#define INSTRUCTION_ADDRESS_SIZE 4 // 4 bits per instruction

// Type aliases
typedef uint8_t byte_t;     // Alias for a byte
typedef uint16_t address_t; // Alias for a memory address of 16 bits
typedef uint32_t word_t;    // Alias for a word of 32 bits

typedef enum
{
    DATA_PROCESSING,      // Data Processing instruction
    MULTIPLY,             // Multiply instruction
    SINGLE_DATA_TRANSFER, // Single Data Transfer instruction
    BRANCH_2,               // Branch instruction
    /* CHANGE BRANCH 2 later to be part of emulate */
    HALT,                 // Halt (All zero) instruction
    NOT_PRESENT,          // Null instruction
    SPECIAL               // Special instruction (lsl, andeq)
} instruction_type_t;

typedef enum
{
    AND = 0x0, // Logical AND
    EOR = 0x1, // Logical XOR
    SUB = 0x2, // Subtract
    RSB = 0x3, // Reversed subtraction
    ADD = 0x4, // Addition
    TST = 0x8, // as AND, without result
    TEQ = 0x9, // as EOR, without result
    CMP = 0xA, // as SUB, without result
    ORR = 0xC, // Logical OR
    MOV = 0xD, // Copy to register
} opcode_t;

typedef enum
{
    EQ = 0x0, // Equal
    NE = 0x1, // Not equal
    GE = 0xA, // Greater or equal
    LT = 0xB, // Less than or equal
    GT = 0xC, // Greater than
    LE = 0xD, // Less than or equal
    AL = 0xE, // Always
} cond_t;

typedef enum
{
    LSL = 0x0, // Logical left shift
    LSR = 0x1, // Logical right shift
    ASR = 0x2, // Arithmetic right shift
    ROR = 0x3, // Rotate right
} shift_t;

#endif //ARMV8_47_GLOBAL_VAR_H
