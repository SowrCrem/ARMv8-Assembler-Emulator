//
// Created by Vivian Lopez on 07/06/2023.
//

#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
uint64_t OULL;

#define NO_GENERAL_REGISTERS 31
#define NO_ELEMENTS 200000000000000000//((int) pow(2,15)) // This constant is used to store the size of memory

typedef enum {ZR, PC, SP} specialRegisters;

// The following 4 structs represent the memory and registers

struct memory {
    uint32_t memory[NO_ELEMENTS];
};

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


struct memory constructMemory() {
    struct memory memory;
    memset(memory.memory, 0, NO_GENERAL_REGISTERS);
    return memory;
}

struct generalRegister constructGeneralRegister() {
    struct generalRegister generalRegisters;
    generalRegisters.mode = 64;
    memset(generalRegisters.data, 0, NO_GENERAL_REGISTERS);
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
    pstate.Z = false;
    pstate.V = false;
    return pstate;
}

struct generalRegister generalRegisters;
struct specialRegister pc;
struct specialRegister zr;
struct PSTATE pstate;
struct memory memory;

int main(void) {
    // Constructs all the registers
    generalRegisters = constructGeneralRegister();
    pc = constructPC();
    zr = constructZR();
    pstate = constructPSTATE();
    memory = constructMemory();
}

uint32_t readMemory(uint32_t address) {
    return memory.memory[address];
}

void writeMemory(uint32_t data, uint32_t address) {
    memory.memory[address] = data;
}

uint32_t* getMemory() {
    return memory.memory;
}

uint64_t readGeneral(int regNum, int mode) {
    return generalRegisters.data[regNum];
};

void writeGeneral(int regNum,uint64_t data, int mode) {
    pc.mode = mode;
    if (pc.mode == 32) {
        generalRegisters.data[regNum] = (data >> 32) | OULL;
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
    if (pc.mode == 32) {
        pc.data = (data >> 32) | OULL;
    } else {
        pc.data = data;
    }
};

void writePC64(uint64_t data, int mode) {
    if (mode == 64 && pc.mode == 32)  {
        printf("Cannot write 64 bit data to 32 bit register");
        exit(1);
    }
    pc.mode = mode;
    if (pc.mode == 32) {
        pc.data = (data >> 32) | OULL;
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