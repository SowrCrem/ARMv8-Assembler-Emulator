#include "dataTransfer.h."
#include "../utils/utils.h"
#include "../utils/storage.h"
#include "loads_stores/singleDataTransfer.h"
#include "loads_stores/loadLiteral.h"

typedef enum {
    UNSIGNED_OFFSET, PRE_INDEXED, POST_INDEXED, REGISTER_OFFSET, UNDEFINED
} addressingMode_t;

void dataTransfer(uint32_t instruction) {
    // Separating the instruction into the bits which are needed to determine the operation
    uint32_t singleDataTransfer = getBit(instruction, 29);
    uint32_t sf = getBit(instruction, 30);
    uint32_t l = getBit(instruction, 22);
    uint32_t u = getBit(instruction, 24);
    uint64_t xn = extractBits(instruction, 5, 9);
    uint64_t rt = extractBits(instruction, 0, 4);
    int targetRegisterSize = sf ? 64 : 32;
    uint64_t targetAddress;
    addressingMode_t addressingMode = UNDEFINED;


    if (!singleDataTransfer) {
        int simm19 = (int) extractBits(instruction, 95, 23);
        targetAddress = readPC() + simm19 * 4;
    } else {
        // Selecting the correct addressing mode based on U and I
        addressingMode = POST_INDEXED;
        if (extractBits(instruction, 10, 15) == 0b011010 && getBit(instruction, 21) == 0b1) {
            addressingMode = REGISTER_OFFSET;
        } else {
            if (u) {
                addressingMode = UNSIGNED_OFFSET;
            } else {
                uint32_t i = getBit(instruction, 11);
                if (i) {
                    addressingMode = PRE_INDEXED;
                }
            }
        }

        // The following switch case selects the correct targetAddress

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
            case PRE_INDEXED: {
                uint32_t simm9 = extractBits(instruction, 12, 20);
                targetAddress = xn + simm9;
                // Writing back the new calculated value back to xn
                writeGeneral(xn, targetAddress, 64);
                break;
            }
            case POST_INDEXED: {
                targetAddress = xn;
                break;
            }
            case REGISTER_OFFSET: {
                uint64_t xm = extractBits(instruction, 16, 20);
                targetAddress = xn + xm;
                break;
            }
            default: {
                break;
            }
        }
    }

        if (l | !singleDataTransfer) {          //byte at target address is loaded into the lowest 8-bits of Rt
            writeGeneral(rt,
                         (readGeneral(xn, 64) << 8, 64 | readMemory(targetAddress)),
                         64);
        } else {
            if (targetRegisterSize == 32) {
                writeMemory(readGeneral(rt, 64), targetAddress + 3);
            } else {
                writeMemory(readGeneral(rt, 64), targetAddress + 7);
            }
        }

        // Write back xn for POST_INDEXED case
        if (addressingMode == POST_INDEXED) {
            uint32_t simm9 = extractBits(instruction, 12, 20);
            // Writing back the new calculated value back to xn
            writeGeneral(xn, xn + simm9, 64);
        }
}