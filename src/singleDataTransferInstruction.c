//
// Created by Vivian Lopez on 08/06/2023.
//

#include <stdint.h>
#include "singleDataTransferInstruction.h"
#include "utils.c"
#include "register.c"

typedef enum {load, store} dataTransfer_t;
typedef enum {unsignedOffset, preIndexed, postIndexed, registerOffset, literal} addressingMode_t;

void singleDataTransfer(uint32_t instruction) {
    // Separating the instruction into the bits which are needed to determine the operation
    uint32_t sf = getBit(instruction, 30);
    uint32_t l = getBit(instruction, 22);
    uint32_t u = getBit(instruction, 24);
    uint64_t xn = extractBits(instruction, 5, 9);

    int targetRegisterSize = sf ? 64 : 32;
    dataTransfer_t dataTransferType = l ? load : store;

    // Selecting the correct addressing mode based on U and I
    addressingMode_t addressingMode;
    if (u) {
        addressingMode = unsignedOffset;
    } else {
        uint32_t i = getBit(instruction, 11);
        if (i) {
            addressingMode = preIndexed;
        } else {
            addressingMode = postIndexed;
        }
    }
    // Not sure how to set register offset

    // The following switch case selects the correct targetAddress
    uint64_t targetAddress;
    switch (addressingMode) {
        case unsignedOffset: {
            uint32_t uoffset;
            uint32_t imm12 = extractBits(instruction, 10, 21);
            if (targetRegisterSize == 64) {
                uoffset = imm12 * 8;
            } else {
                uoffset = imm12 * 4;
            }
            targetAddress = xn + (uint64_t) uoffset;
            break;
        }
        case preIndexed: {
            uint32_t simm9 = extractBits(instruction, 12, 20);
            targetAddress = xn + simm9;
            // Writing back the new calculated value back to xn
            generalRegisters.data[xn] = targetAddress;
            break;
        }
        case postIndexed: {
            targetAddress = xn;
            break;
        }
        case registerOffset: {
            uint64_t xm = extractBits(instruction, 16, 20);
            targetAddress = xn + xm;
            break;
        }
        default: {
            break;
        }
    }

    // Remember to write back xn for postIndexed case
    if (targetRegisterSize == 64) {
        memory[]
    }


}