//
// Created by Vivian Lopez on 08/06/2023.
//

#include <stdint.h>
#include "singleDataTransfer.h"
#include "utils.c"

typedef enum {load, store} dataTransfer_t;
typedef enum {unsignedOffset, preIndexed, postindexed, registerOffset, literal} addressingMode_t;

void singleDataTransfer(uint32_t instruction) {
    // Separating the instruction into the bits which are needed to determine the operation
    uint32_t sf = getBit(instruction, 30);
    uint32_t l = getBit(instruction, 22);
    uint32_t u = getBit(instruction, 24);
    dataTransfer_t dataTransferType = l ? load : store;
    addressingMode_t addressingMode;
    if (u) {
        addressingMode = unsignedOffset;
    } else {
        uint32_t i = getBit(instruction, 11);
        if (i) {
            addressingMode = preIndexed;
        } else {
            addressingMode = postindexed;
        }
    }

}