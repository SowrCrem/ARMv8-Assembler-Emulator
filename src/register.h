//
// Created by Vivian Lopez on 07/06/2023.
//

#ifndef ARMV8_47_REGISTER_H
#define ARMV8_47_REGISTER_H

// Function Declaration
uint64_t readGeneral(int regNum, int mode);
void writeGeneral(int regNum,uint64_t data, int mode);
uint64_t readPC();
void writePC32(uint32_t data, int mode);
void writePC64(uint64_t data, int mode);
uint64_t readZR();
void writeN(bool val);
void writeV(bool var);
void writeZ(bool var);
void writeC(bool var);

#endif //ARMV8_47_REGISTER_H
