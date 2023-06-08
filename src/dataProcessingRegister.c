//
// Created by Vivian Lopez on 07/06/2023.
//

#include "dataProcessingRegister.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "registerAndMemory.h"

// BITWISE SHIFT FUNCTIONS

// Logical Shift Left (LSL) for uint64_t
uint64_t lsl64(uint64_t value, uint32_t shift) {
    return value << shift;
}

// Logical Shift Left (LSL) for uint32_t
uint32_t lsl32(uint32_t value, uint32_t shift) {
    return value << shift;
}

// Logical Shift Right (LSR) for uint64_t
uint64_t lsr64(uint64_t value, uint32_t shift) {
    return value >> shift;
}

// Logical Shift Right (LSR) for uint32_t
uint32_t lsr32(uint32_t value, uint32_t shift) {
    return value >> shift;
}

// Arithmetic Shift Right (ASR) for int64_t
int64_t asr64(int64_t value, uint32_t shift) {
    int64_t sign_bit = value < 0 ? (int64_t)1 << 63 : 0;    // Extract sign bit
    int64_t shifted_value = value >> shift;                 // Perform the shift
    int64_t sign_extension = sign_bit >> shift;             // Extend the sign bit
    return shifted_value | sign_extension;                  // Combine the shifted value and sign extension
}

// Arithmetic Shift Right (ASR) for int32_t
int32_t asr32(int32_t value, uint32_t shift) {
    int32_t sign_bit = value < 0 ? (int32_t)1 << 31 : 0;    // Extract sign bit
    int32_t shifted_value = value >> shift;                 // Perform the shift
    int32_t sign_extension = sign_bit >> shift;             // Extend the sign bit
    return shifted_value | sign_extension;                  // Combine the shifted value and sign extension
}

// Rotate Right (ROR) for uint64_t
uint64_t ror64(uint64_t value, uint32_t shift) {
    uint32_t num_bits = sizeof(uint64_t) * 8;
    shift %= num_bits;
    return (value >> shift) | (value << (num_bits - shift));
}

// Rotate Right (ROR) for uint32_t
uint32_t ror32(uint32_t value, uint32_t shift) {
    uint32_t num_bits = sizeof(uint32_t) * 8;
    shift %= num_bits;
    return (value >> shift) | (value << (num_bits - shift));
}


void arithmetic(uint32_t inst) {

}

void bit_logic(uint32_t inst) {

}

void multiply(uint32_t inst) {

}

void choose(uint32_t instruction) {
    bool m_value = getMSB(instruction << 3);
    if (m_value) {
        multiply(instruction);
    } else {
        bool opr_value = getMSB(instruction << 7);
        if (opr_value) {
            arithmetic(instruction);
        } else {
            bit_logic(instruction);
        }
    }

    /* code */
}