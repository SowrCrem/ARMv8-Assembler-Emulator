//
// Created by saura on 20/06/2023.
//
// Global constant, enum and type definitions

#ifndef ARMV8_47_GLOBAL_VAR_H
#define ARMV8_47_GLOBAL_VAR_H

// emulate.c constants
#define NO_ELEMENTS ((int) pow(2,21))       // Used to store the size of memory
#define TERMINATE_INSTRUCTION 0x8a000000    // AND x0 x0 x0
#define NO_OP_INSTRUCTION 0xd503203f

#endif //ARMV8_47_GLOBAL_VAR_H
