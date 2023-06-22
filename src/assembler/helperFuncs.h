#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "dpi.h"
#include "dpr.h"
#include "branch.h"
#include "sdtorll.h"

typedef struct
{
    bool N;
    bool Z;
    bool C;
    bool V;
} PSTATE;

typedef struct
{
    uint64_t general[31];
    uint64_t zr, sp, pc;
    PSTATE ps;
} state;

extern uint8_t *byteMemory;
extern state machineState;

extern uint32_t shiftAndKeep(uint32_t instruction, int shift, int keep);

extern uint64_t shiftAndKeep64(uint64_t instruction, int shift, int keep);

extern int64_t shiftAndKeepSigned(uint32_t instruction, int shift, int keep);

extern uint64_t signExtend(int num, int numSize);

extern void setMemory32(uint32_t address, uint32_t value);

extern void setMemory64(uint32_t address, uint64_t value);

extern uint32_t readMemory32(uint32_t address);

extern uint64_t readMemory64(uint32_t address);

extern int splice(uint32_t instructions, int leftBit, int rightBit);

extern void writeRegister32(int num, uint32_t value);

extern void writeRegister64(int num, uint64_t value);

extern uint32_t readRegister32(int num);

extern uint64_t readRegister64(int num);

extern void setPSFlags(int result, uint64_t op1, uint64_t op2);

extern void arithmetic(int opc, uint64_t op1, uint64_t op2, int rd, int sf);

extern void branch(uint32_t instruction);

extern void dpi(uint32_t instruction);

extern void dpr(uint32_t instruction);

extern void loadorstore(uint32_t instruction);

extern int findRd(uint32_t instruction);

extern int findSf(uint32_t instruction);

extern int findOpc(uint32_t instruction);

extern int findRn(uint32_t instruction);

extern int findImm12(uint32_t instruction);

extern void byteReadFile(char name[]);

extern void writeOutput(char path[]);