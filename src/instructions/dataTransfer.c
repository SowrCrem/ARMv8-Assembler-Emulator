#include "dataTransfer.h."
#include "../utils/utils.h"
#include "../utils/storage.h"
#include "loads_stores/singleDataTransfer.h"
#include "loads_stores/loadLiteral.h"

typedef enum {
    load, store
} dataTransfer_t;

typedef enum {
    UNSIGNED_OFFSET, preIndexed, postIndexed, registerOffset, loadLiteral
} addressingMode_t;

void dataTransfer(uint32_t instruction) {
    // Separating the instruction into the bits which are needed to determine the operation
    uint32_t sf = getBit(instruction, 30);
    uint32_t l = getBit(instruction, 22);
    uint32_t u = getBit(instruction, 24);
    uint64_t xn = extractBits(instruction, 5, 9);
    uint64_t rt = extractBits(instruction, 0, 4);

    int targetRegisterSize = sf ? 64 : 32;
    dataTransfer_t dataTransferType = l ? load : store;

    // Selecting the correct addressing mode based on U and I
    addressingMode_t addressingMode = postIndexed;
    if (extractBits(instruction, 10, 15) == 0b011010 && getBit(instruction, 21) == 0b1) {
        addressingMode = registerOffset;
    } else {
        if (u) {
            addressingMode = UNSIGNED_OFFSET;
        } else {
            uint32_t i = getBit(instruction, 11);
            if (i) {
                addressingMode = preIndexed;
            }
        }
    }

    // The following switch case selects the correct targetAddress
    uint64_t targetAddress;
    switch (addressingMode) {
        case UNSIGNED_OFFSET: {
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
            writeGeneral(xn, targetAddress, 64);
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
    // byte at target address is loaded into the lowest 8-bits of Rt
    writeGeneral(rt,
                 (readGeneral(xn, 64) << 8, 64 | readMemory(targetAddress)),
                 64);
    if (targetRegisterSize == 32) {
        writeMemory(readGeneral(rt, 64), targetAddress + 3);
    } else {
        writeMemory(readGeneral(rt, 64), targetAddress + 7);
    }

    // Write back xn for postIndexed case
    if (addressingMode == postIndexed) {
        uint32_t simm9 = extractBits(instruction, 12, 20);
        // Writing back the new calculated value back to xn
        writeGeneral(xn, xn + simm9, 64);
    }
}