//
// Created by Vivian Lopez on 07/06/2023.
//

#ifndef ARMV8_47_REGISTERANDMEMORY_H
#define ARMV8_47_REGISTERANDMEMORY_H

// Function Declaration
uint64_t readGeneral(int regNum, int mode);
void writeGeneral(int regNum,uint64_t data, int mode);
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
uint32_t* getMemory(void);
void construct(void);

#endif //ARMV8_47_REGISTERANDMEMORY_H
