// Definition of a type to hold an arbitrary instruction

#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <stdbool.h>
#include "../global_var.h"

typedef struct
{
    instruction_type_t type;

    byte_t cond;     // Condition code (DP, Multiply, SDT, Branch)
    opcode_t opcode; // Opcode (DP)

    byte_t rd; // Register Rd (DP, Multiply, SDT)
    byte_t rn; // Register Rn (DP, Multiply, SDT)
    byte_t rm; // Register Rm (Multiply)
    byte_t rs; // Register Rs (Multiply)

    bool use_rs; // True if shift specified by Rs

    bool flag_i; // Immediate bit (DP, SDT)
    bool flag_a; // Accumulate bit (Multiply)
    bool flag_s; // Set Condition Codes bit (Multiply)
    bool flag_l; // Load/Store bit (SDT)
    bool flag_p; // Pre/Post indexing bit (SDT)
    bool flag_u; // Up bit (SDT)

    shift_t shift_type;  // Type of shift performed (DP)
    byte_t shift_amount; // Amount to shift by (DP)

    uint32_t imm;    // Immediate value (DP)
    uint32_t offset; // Offset (SDT, Branch)
} instruction_t;

#endif