//
// Created by Vivian Lopez on 07/06/2023.
//

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "utils.h"

#define NO_GENERAL_REGISTERS 31
#define NO_ELEMENTS pow(2,15) // This constant is used to store the size of memory

typedef enum {ZR, PC, SP} specialRegisters;

// The following 3 structs represent the general and special registers

struct generalRegister {
    int mode;
    uint64_t data[NO_GENERAL_REGISTERS];
};

struct specialRegister {
    specialRegisters name;
    uint64_t data;
    int mode;
};

struct PSTATE {
    bool N;
    bool Z;
    bool C;
    bool V;
};

struct generalRegister constructGeneralRegister() {
    struct generalRegister generalRegisters;
    generalRegisters.mode = 64;
    memset(generalRegisters.data, 0, NO_GENERAL_REGISTERS * sizeof(uint64_t));
    return generalRegisters;
}

struct specialRegister constructPC() {
    struct specialRegister pc;
    pc.name = PC;
    pc.data = 0x0;
    pc.mode = 64;
    return pc;
}

struct specialRegister constructZR() {
    struct specialRegister zr;
    zr.name = ZR;
    zr.data = 0;
    zr.mode = 64;
    return zr;
}

struct PSTATE constructPSTATE() {
    struct PSTATE pstate;
    pstate.C = false;
    pstate.N = false;
    pstate.Z = true;
    pstate.V = false;
    return pstate;
}

// Modelling memory using malloc()
uint32_t* memory;

void initializeMemory() {
    memory = (uint32_t*)malloc(NO_ELEMENTS * sizeof(uint32_t));
    if (memory == NULL) {
        printf("Error: Failed to allocate memory\n");
        exit(1);
    }
}

struct generalRegister generalRegisters;
struct specialRegister pc;
struct specialRegister zr;
struct PSTATE pstate;

void construct(void) {
    // Constructs all the registers
    generalRegisters = constructGeneralRegister();
    pc = constructPC();
    zr = constructZR();
    pstate = constructPSTATE();
    initializeMemory();
}

uint32_t readMemory(uint32_t address) {
    // returns data at address
    if (address < NO_ELEMENTS) {
        return memory[address];
    } else {
        printf("Error: Invalid memory address %d\n", address);
        return 0;
    }
}

void writeMemory(uint32_t data, uint32_t address) {
    // write data to address
    if (address < NO_ELEMENTS) {
        memory[address] = data;
    } else {
        printf("Error: Invalid memory address %d\n", address);
    }
}

uint32_t* getMemory() {
    // returns basal pointer to array
    return memory;
}

void freeMemory() {
    free(memory);
}

uint64_t readGeneral(uint64_t regNum, int mode) {
    return generalRegisters.data[regNum];
};

void writeGeneral(uint64_t regNum, uint64_t data, int mode) {
    pc.mode = mode;
    if (pc.mode == 32) {
        generalRegisters.data[regNum] = (data >> 32) | 0ULL;
    } else {
        generalRegisters.data[regNum] = data;
    }
};

uint64_t readPC() {
    if (pc.mode == 32) {
        return extractBits(pc.data, 31, 0);
    }
    return pc.data;
};

void writePC32(uint32_t data, int mode) {
    pc.mode = mode;
    pc.data = data;
}

void writePC64(uint64_t data, int mode) {
    if (mode == 64 && pc.mode == 32)  {
        printf("Cannot write 64 bit data to 32 bit register");
        exit(1);
    }
    pc.mode = mode;
    if (pc.mode == 32) {
        pc.data = (data >> 32) | 0ULL;
    } else {
        pc.data = data;
    }
};

uint64_t readZR() {
    return 0;
};

bool readN() {
    return pstate.N;
}

void writeN(bool val) {
    if (val) {
        pstate.N = true;
    } else {
        pstate.N = false;
    }
}

bool readZ() {
    return pstate.Z;
}

void writeZ(bool val) {
    if (val) {
        pstate.Z = true;
    } else {
        pstate.Z = false;
    }
}

bool readC() {
    return pstate.C;
}

void writeC(bool val) {
    if (val) {
        pstate.C = true;
    } else {
        pstate.C = false;
    }
}

bool readV() {
    return pstate.V;
}

void writeV(bool val) {
    if (val) {
        pstate.V = true;
    } else {
        pstate.V = false;
    }
}