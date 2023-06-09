#ifndef ARMV8_47_STORAGE_H
#define ARMV8_47_STORAGE_H

#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// Function Declaration
uint64_t readGeneral(uint64_t regNum, int mode);

void writeGeneral(uint64_t regNum, uint64_t data, int mode);

uint64_t readPC();

void writePC32(uint32_t data, int mode);

void writePC64(uint64_t data, int mode);

uint64_t readZR();

bool readN();

void writeN(bool val);

bool readV();

void writeV(bool var);

bool readZ();

void writeZ(bool var);

bool readC();

void writeC(bool var);

uint32_t readMemory(uint32_t address);

void writeMemory(uint32_t data, uint32_t address);

uint32_t *getMemory(void);

void construct(void);

void freeMemory();

#endif //ARMV8_47_STORAGE_H
