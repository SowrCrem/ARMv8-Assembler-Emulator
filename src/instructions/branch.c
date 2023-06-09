#include "branch.h"
#include "../utils/storage.h"

typedef enum {unconditional, reg, conditional} branch_t;

int64_t signExtendTo64(int64_t number, int n) {
    // Extract the sign bit (MSB)
    int64_t signBit = number >> (n - 1);
    int64_t signExtendedNumber = number;

    if (signBit != 0) {
        // If the sign bit is set (negative number), perform sign extension
        int64_t signExtensionMask = (1LL << (64 - n)) - 1;
        signExtendedNumber |= (signExtensionMask << n);
    }

    return signExtendedNumber;
}

void b(uint32_t instruction) {  // Unconditional branch to (PC + offset)
    // Read PC register
    uint64_t programCounter = readPC();
    // Extract offset (simm26) from instruction
    uint32_t simm26 = extractBits(instruction, 0, 25);
    // Sign extend offset to 64-bit
    uint64_t signExtendedOffset = signExtendTo64(simm26 * 4, 26);
    // Apply offset to PC register
    writePC64(programCounter + signExtendedOffset, 64);
}

void br(uint32_t instruction) {
    // Branch to address in register xn
    uint8_t xn = extractBits(instruction, 5, 9);
    if (xn == 0b11111) {
        // encodes zero register (xzr)
        return;     // don't need to handle this case
    }

    // Write xn to PC
    writePC64(xn, 64);  // not sure if I need to sign extend to 32 or 64 bits
}

void bcond(uint32_t instruction) {      // Branch to (PC + offset) iff cond
    // Read PC register
    uint64_t programCounter = readPC();
    // Extract cond bits
    uint8_t cond = extractBits(instruction, 0, 4);
    bool satisfied = 0;
    switch (cond) {
        case 0b0000: {
            if (readZ() == 1) satisfied = 1;
            break;
        }
        case 0b001: {
            if (readZ() == 0) satisfied = 1;
            break;
        }
        case 0b1010: {
            if (readN() == readV()) satisfied = 1;
            break;
        }
        case 0b1011: {
            if (readN() != readV()) satisfied = 1;
            break;
        }
        case 0b1100: {
            if (readZ() == 0 && readN() == readV()) satisfied = 1;
            break;
        }
        case 0b1101: {
            if (!(readZ() == 0 && readN() == readV())) satisfied = 1;
            break;
        }
        case 0b1110: {
            satisfied = 1;
            break;
        }
        default:
            break;
    }

    if (satisfied == 1) {
        // Extract offset (simm19) from instruction
        uint32_t simm19 = extractBits(instruction, 5, 23);
        // Sign extend offset to 64-bit
        uint64_t signExtendedOffset = signExtendTo64(simm19 * 4, 19);
        // Apply offset to PC register
        writePC64(programCounter + signExtendedOffset, 64);
    }
}

void branch( uint32_t instruction ) {
    // PRE: bits 28-25 = 101x

    bool msb = getMSB(instruction);
    uint32_t bits30to26 = extractBits(instruction, 26, 30);
    uint32_t bits30to25 = extractBits(instruction, 25, 30);
    branch_t branch;

    switch (msb) {
        case 0:
            // Unconditional branch
            if (bits30to26 == 0b00101) branch = unconditional;
            else if (bits30to25 == 0b101010) branch = conditional;
            break;
        case 1:
            if (bits30to25 == 0b101011) branch = reg;
            break;
        default:
            // branch is not set
            break;
    }

    switch (branch) {
        case unconditional:
            b(instruction);
            break;
        case reg:
            br(instruction);
            break;
        case conditional:
            bcond(instruction);
            break;
        default:
            break;
    }
}
