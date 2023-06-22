#include "helperFuncs.h"
#include <assert.h>

uint8_t *byteMemory;
state machineState;

#define TOT_INSTRUCT_NUM pow(2, 21)

#define rdKeep 5
#define rdShift 0
#define sfKeep 1
#define sfShift 31
#define opcKeep 2
#define opcShift 29
#define rnShift 5
#define rnKeep 5
#define imm12Keep 12
#define imm12Shift 10
#define hwKeep 2
#define hwShift 21
#define imm16Keep 16
#define imm16Shift 5
#define opiKeep 3
#define opiShift 23
#define xShift 5
#define xKeep 1
#define shKeep 1
#define shShift 22
#define raShift 4
#define raKeep 4
#define shiftShiftAmount 1
#define shiftKeep 2

// Retrieves certain bit range of an integer
// Potentially turn into a splice function???
uint32_t shiftAndKeep(uint32_t instruction, int shift, int keep)
{
    return (instruction >> shift) & ((int)pow(2, keep) - 1);
}

int64_t shiftAndKeepSigned(uint32_t instruction, int shift, int keep)
{

    int32_t shifted = (instruction >> shift) & ((int)pow(2, keep) - 1);

    if (shifted >= pow(2, keep - 1))
    {
        return ((int64_t)(0x3ffff & shifted)) - pow(2, keep - 1);
    }
    return (int64_t)shifted;
}

uint64_t shiftAndKeep64(uint64_t instruction, int shift, int keep)
{
    return (instruction >> shift) & ((int)pow(2, keep) - 1);
}

uint64_t signExtend(int num, int numSize)
{
    uint64_t mask = 0x7FFFFFFFFFFFFFFF; // Max size
    numSize--;
    bool negative = shiftAndKeep(num, numSize, 1) == 1;
    if (negative)
    {
        mask = ((mask >> numSize) << numSize) + num;
    }
    else
    {
        mask = num;
    }
    return mask;
}

void setMemory32(uint32_t address, uint32_t value)
{
    int valueKeep = 8;
    for (int i = 0; i < 4; i++)
    {
        int valueShift = i * 8;
        byteMemory[address + i] = (uint8_t)shiftAndKeep(value, valueShift, valueKeep);
    }
    printf("Writing 32 bit value 0x%x to memory address 0x%x\n", value, address);
}

void setMemory64(uint32_t address, uint64_t value)
{
    int valueKeep = 8;
    for (int i = 0; i < 8; i++)
    {
        int valueShift = i * 8;
        byteMemory[address + i] = (uint8_t)shiftAndKeep64(value, valueShift, valueKeep);
    }
    printf("Writing 64 bit value 0x%lx to memory address 0x%x\n", value, address);
}

uint32_t readMemory32(uint32_t address)
{
    uint32_t total = 0;
    for (int i = 0; i < 4; i++)
    {
        total += byteMemory[address + i] << (i * 8);
    }
    return total;
}

uint64_t readMemory64(uint32_t address)
{
    uint64_t total = 0;
    // for (int i = 0; i < 8; i++) {
    //   total += byteMemory[address+i] << (i*8);
    // }
    total = (uint64_t)readMemory32(address) + (((uint64_t)readMemory32(address + 4)) << 32);
    return total;
}

int splice(uint32_t instruction, int leftBit, int rightBit);

void writeRegister32(int num, uint32_t value)
{
    machineState.general[num] = (uint64_t)value;
    printf("Writing 32-bit value 0x%x to W%d\n", value, num);
}

void writeRegister64(int num, uint64_t value)
{
    machineState.general[num] = value;
    printf("Writing 64-bit value 0x%lx to X%d\n", value, num);
}

uint32_t readRegister32(int num)
{
    return (uint32_t)((machineState.general[num] << 32) >> 32);
}

uint64_t readRegister64(int num)
{
    return machineState.general[num];
}

void setPSFlags(int result, uint64_t op1, uint64_t op2)
{
    printf("setPSFlags, %d, %lx, %lx\n", result, op1, op2);
    int64_t op1_ = op1;
    int64_t op2_ = op2;

    machineState.ps.N = result < 0;  // N is set to the sign bit of the result
    machineState.ps.Z = result == 0; // Z is set only when the result is all zero
    machineState.ps.V =
        (op1_ > 0 && op2_ > 0 && result < 0) || // if both numbers are positive but their sum is negative
        (op1_ < 0 && op2_ < 0 && result > 0);   // if both numbers are negative but their sum is positive
}

int findRd(uint32_t instruction)
{
    return shiftAndKeep(instruction, rdShift, rdKeep);
}

int findSf(uint32_t instruction)
{
    return shiftAndKeep(instruction, sfShift, sfKeep);
}

int findOpc(uint32_t instruction)
{
    return shiftAndKeep(instruction, opcShift, opcKeep);
}

int findRn(uint32_t instruction)
{
    return shiftAndKeep(instruction, rnShift, rnKeep);
}

int findImm12(uint32_t instruction)
{
    return shiftAndKeep(instruction, imm12Shift, imm12Keep);
}

void arithmetic(int opc, uint64_t op1, uint64_t op2, int rd, int sf)
{
    printf("Called arithmetic...\n");
    uint64_t max = (sf == 1) ? (__UINT64_MAX__) : (__UINT32_MAX__);
    enum dpiImmOpc
    {
        ADD,
        ADDS,
        SUB,
        SUBS
    };
    switch (opc)
    {
    case ADD:
        if (sf == 0)
        {
            printf("32 bit mode ADD of %d and %d, storing to W%d\n", (int)op1, (int)op2, rd);
            writeRegister32(rd, ((int32_t)(uint32_t)op1) + ((int32_t)(uint32_t)op2));
        }
        else
        {
            printf("64 bit mode ADD of %d and %d, storing to X%d\n", (int)op1, (int)op2, rd);
            writeRegister64(rd, op1 + op2);
        }
        break;

    case ADDS:
        if (sf == 0)
        {
            printf("32 bit mode ADDS of %d and %d, storing to W%d\n", (int)op1, (int)op2, rd);
            writeRegister32(rd, ((int32_t)(uint32_t)op1) + ((int32_t)(uint32_t)op2));
        }
        else
        {
            printf("64 bit mode ADDS of %d and %d, storing to X%d\n", (int)op1, (int)op2, rd);
            writeRegister64(rd, op1 + op2);
        }
        uint64_t currValue = (sf == 0) ? ((uint64_t)readRegister32(rd)) : readRegister64(rd);
        // sets the N, Z and V flags
        setPSFlags(currValue, op1, op2);
        // sets the C Flag by checking for unsigned overflow
        machineState.ps.C = (op2 > 0) && (op1 > max - op2);
        break;

    case SUB:
        if (sf == 0)
        {
            printf("32 bit mode SUB of %d and %d, storing to W%d\n", (int)op1, (int)op2, rd);
            writeRegister32(rd, ((int32_t)(uint32_t)op1) - ((int32_t)(uint32_t)op2));
        }
        else
        {
            printf("64 bit mode SUB of %d and %d, storing to X%d\n", (int)op1, (int)op2, rd);
            writeRegister64(rd, op1 - op2);
        }
        break;

    case SUBS:
        printf("SUBS\n");
        if (sf == 0)
        {
            printf("32 bit mode SUBS of %d and %d, storing to W%d\n", (int)op1, (int)op2, rd);
            writeRegister32(rd, ((int32_t)(uint32_t)op1) - ((int32_t)(uint32_t)op2));
        }
        else
        {
            printf("64 bit mode SUBS of %d and %d, storing to X%d\n", (int)op1, (int)op2, rd);
            writeRegister64(rd, op1 - op2);
        }
        currValue = (sf == 0) ? ((uint64_t)readRegister32(rd)) : readRegister64(rd);
        // sets the N, Z and V flags, where conditions are the same for addition and subtraction
        setPSFlags(currValue, op1, op2 * -1);
        // sets the C Flag by checking for unsigned underflow but negating the result as given in spec
        machineState.ps.C = !((op2 > 0) && (op1 < (max * -1) - 1 + op1));
        break;

    default:
        printf("UNKNOWN ACTION\n");
        exit(43);
    }
}

// reads binary file into byteMemory array
void byteReadFile(char name[])
{
    FILE *file;
    file = fopen(name, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "cat: can't open %s\n", name);
        exit(11);
    }
    fread(byteMemory, TOT_INSTRUCT_NUM, 1, file);
}

void writeOutput(char path[])
{
    FILE *file;
    file = fopen(path, "w");
    fputs("Registers:\n", file);
    for (int i = 0; i < 31; i++)
    {
        // Uses 0 padding
        fprintf(file, "X%02d = %016lx\n", i, machineState.general[i]);
    }
    fprintf(file, "PC   = %016lx\n", machineState.pc);
    PSTATE ps = machineState.ps;
    char NZCV[4];
    NZCV[0] = ps.N ? 'N' : '-';
    NZCV[1] = ps.Z ? 'Z' : '-';
    NZCV[2] = ps.C ? 'C' : '-';
    NZCV[3] = ps.V ? 'V' : '-';
    fprintf(file, "PSTATE: %s\n", NZCV);
    fprintf(file, "Non-zero memory:\n");
    for (uint32_t i = 0; i < TOT_INSTRUCT_NUM; i += 4)
    {
        if (readMemory32(i) != 0)
        {
            fprintf(file, "0x%08X: %08x\n", i, readMemory32(i));
        }
    }
    fclose(file);
}