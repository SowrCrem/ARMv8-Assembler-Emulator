//
// Created by Vivian Lopez on 07/06/2023.
//

#include "dataProcessingRegister.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "registerAndMemory.h"

void multiply(uint32_t inst) {

}

void arithmetic(uint32_t inst) {

}

void bit_logic(uint32_t inst) {

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