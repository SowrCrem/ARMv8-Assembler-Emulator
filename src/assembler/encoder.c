// Functions for the second pass (binary encoding)

#include "encoder.h"

static word_t encode_data_processing(instruction_t *instruction)
{
    word_t binary = 0x0;

    binary |= (((word_t)instruction->cond) & MASK_4_LSB) << 28;

    binary |= (((word_t)instruction->flag_i) & MASK_1_LSB) << 25;
    binary |= (((word_t)instruction->opcode) & MASK_4_LSB) << 21;
    binary |= (((word_t)instruction->flag_s) & MASK_1_LSB) << 20;

    if (!((instruction->rn >> 4) & MASK_1_LSB))
    {
        binary |= (((word_t)instruction->rn) & MASK_4_LSB) << 16;
    }

    if (!((instruction->rd >> 4) & MASK_1_LSB))
    {
        binary |= (((word_t)instruction->rd) & MASK_4_LSB) << 12;
    }

    if (instruction->flag_i)
    {
        // Operand2 is an immediate value
        binary |= (((word_t)instruction->imm) & MASK_8_LSB);
        binary |= (((word_t)instruction->shift_amount) & MASK_4_LSB) << 8;
    }
    else
    {
        // Operand2 is a register
        if (instruction->use_rs)
        {
            // Shift specified by a register
            binary |= (((word_t)instruction->rs) & MASK_4_LSB) << 8;
            binary |= MASK_1_LSB << 4;
        }
        else
        {
            // Shift by a constant amount
            binary |= ((instruction->shift_amount) & MASK_5_LSB) << 7;
        }

        binary |= (((word_t)instruction->shift_type) & MASK_2_LSB) << 5;
        binary |= (((word_t)instruction->rm) & MASK_4_LSB);
    }

    return binary;
}

static word_t encode_multiply(instruction_t *instruction)
{
    word_t binary = 0x0;

    binary |= (((word_t)instruction->cond) & MASK_4_LSB) << 28;

    binary |= (((word_t)instruction->flag_a) & MASK_1_LSB) << 21;
    binary |= (((word_t)instruction->flag_s) & MASK_1_LSB) << 20;

    binary |= (((word_t)instruction->rd) & MASK_4_LSB) << 16;

    if (instruction->flag_a)
    {
        binary |= (((word_t)instruction->rn) & MASK_4_LSB) << 12;
    }

    binary |= (((word_t)instruction->rs) & MASK_4_LSB) << 8;

    binary |= 0x9 << 4; // 1001 in bits 7 to 4

    binary |= (((word_t)instruction->rm) & MASK_4_LSB);

    return binary;
}

static word_t encode_single_data_transfer(instruction_t *instruction)
{
    word_t binary = 0x0;

    binary |= (((word_t)instruction->cond) & MASK_4_LSB) << 28;

    binary |= MASK_1_LSB << 26;

    binary |= (((word_t)instruction->flag_i) & MASK_1_LSB) << 25;
    binary |= (((word_t)instruction->flag_p) & MASK_1_LSB) << 24;
    binary |= (((word_t)instruction->flag_u) & MASK_1_LSB) << 23;
    binary |= (((word_t)instruction->flag_l) & MASK_1_LSB) << 20;

    binary |= (((word_t)instruction->rn) & MASK_4_LSB) << 16;
    binary |= (((word_t)instruction->rd) & MASK_4_LSB) << 12;

    if (instruction->flag_i)
    {
        // Offset is a register
        if (instruction->use_rs)
        {
            // Shift specified by a register
            binary |= (((word_t)instruction->rs) & MASK_4_LSB) << 8;
            binary |= MASK_1_LSB << 4;
        }
        else
        {
            // Shift by a constant amount
            binary |= ((instruction->shift_amount) & MASK_5_LSB) << 7;
        }

        binary |= (((word_t)instruction->shift_type) & MASK_2_LSB) << 5;
        binary |= (((word_t)instruction->rm) & MASK_4_LSB);
    }
    else
    {
        // Offset is an immediate value
        binary |= (((word_t)instruction->offset) & MASK_12_LSB);
    }

    return binary;
}

static word_t encode_branch(instruction_t *instruction)
{
    word_t binary = 0x0;

    binary |= (((word_t)instruction->cond) & MASK_4_LSB) << 28;

    binary |= 0xA << 24; // 1010 in bits 27 to 24

    binary |= (((word_t)instruction->offset) & MASK_24_LSB);

    return binary;
}

word_t encode(instruction_t *instruction)
{
    switch (instruction->type)
    {
        case DATA_PROCESSING:
            return encode_data_processing(instruction);
        case MULTIPLY:
            return encode_multiply(instruction);
        case SINGLE_DATA_TRANSFER:
            return encode_single_data_transfer(instruction);
        case BRANCH_2:
            return encode_branch(instruction);
        case HALT:
            return 0x0;
        default:
            fprintf(stderr, "Invalid instruction type.\n");
            return -1;  // Error output
    }
}