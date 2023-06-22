#include "helperFuncs.h"

#define MAX_BIT_INDEX 63

// function to keep the bits between leftBit and rightBit the same but zero everything else
uint64_t spliceAndZero(uint64_t value, int leftBit, int rightBit)
{
    int size = leftBit - rightBit;
    if (leftBit < rightBit)
    {
        printf("spliceAndZero error: Leftbit smaller than rightbit\n");
        return 0;
    }
    return ((value >> rightBit) << (MAX_BIT_INDEX - size)) >> (MAX_BIT_INDEX - leftBit);
};

void dpi(uint32_t instruction)
{
    int rd = findRd(instruction);
    int sf = findSf(instruction);
    int opc = findOpc(instruction);

    int opiShift = 23;
    int opiKeep = 3;
    int opi = shiftAndKeep(instruction, opiShift, opiKeep);

    // when opi = 2, the instruction is arithmetic so carries out instruction accordingly
    if (opi == 2)
    {
        int rn = findRn(instruction);
        int imm12 = findImm12(instruction);

        int shShift = 22;
        int shKeep = 1;
        int sh = shiftAndKeep(instruction, shShift, shKeep);

        // if the shift value is 1, the imm12 value needs to be left shifted by 12
        if (sh == 1)
        {
            imm12 <<= 12;
        }

        // if rn = 11111, the stack pointer in encoded
        if (rn == 0b01111)
        {
            machineState.sp = 1;
        }

        // if rd = 11111 and flags need to be set, the stack pointer is encoded otherwise the zero flag is encoded
        if (rd == 0b11111)
        {
            if (opc == 0 || opc == 3)
            {
                machineState.sp = 1;
            }
            else
            {
                machineState.zr = 0;
            }
        }

        uint64_t op1 = (sf == 0) ? ((uint64_t)readRegister32(rn)) : readRegister64(rn);
        uint64_t op2 = imm12;
        printf("dpiImm12\n");
        arithmetic(opc, op1, op2, rd, sf);
    }
    // when opi = 5, the instruction is a wide move so carries out instruction accordingly
    else if (opi == 5)
    {
        int hwKeep = 2;
        int hwShift = 21;
        int hw = shiftAndKeep(instruction, hwShift, hwKeep);

        int imm16Keep = 16;
        int imm16Shift = 5;
        uint64_t imm16 = shiftAndKeep(instruction, imm16Shift, imm16Keep);

        imm16 <<= (hw * 16);
        enum dpiImmOpc
        {
            MOVN = 0b0,
            MOVZ = 0b010,
            MOVK = 0b011
        };

        switch (opc)
        {
        case MOVN:
            if (sf == 0)
            {
                writeRegister32(rd, ~imm16);
            }
            else
            {
                writeRegister64(rd, ~imm16);
            }
            break;

        case MOVZ:
            if (sf == 0)
            {
                writeRegister32(rd, imm16);
            }
            else
            {
                writeRegister64(rd, imm16);
            }
            break;

            // finds the left and right side of the original value of the register that need to stay the same
            // then does a bitwise or with the imm16 value and stores it in the register
        case MOVK:
            uint64_t left = spliceAndZero(readRegister64(rd), MAX_BIT_INDEX, (hw + 1) * 16);
            uint64_t right = spliceAndZero(readRegister64(rd), (hw * 16) - 1, 0);
            uint64_t value = left | imm16 | right;
            if (sf == 0)
            {
                writeRegister32(rd, value);
            }
            else
            {
                writeRegister64(rd, value);
            }
            break;

        default:
            printf("UNKNOWN opc for DPI\n");
        }
    }
    else
    {
        printf("DPI unknown instruction type\n");
    }
}