#include "helperFuncs.h"

void dpr_multiply(uint32_t operand, uint32_t rd, uint32_t rn, uint32_t rm, uint32_t sf)
{
    printf("DPR multiply\n \n");
    int xShift = 5;
    int xKeep = 1;
    int x = shiftAndKeep(operand, xShift, xKeep);

    int raShift = 4;
    int raKeep = 4;
    int ra = shiftAndKeep(operand, raShift, raKeep);

    if (ra == 31)
    {
        machineState.zr = 1;
    }
    else
    {
        if (sf == 0)
        {
            int32_t res;
            if (x == 0)
            {
                res = (int32_t)readRegister32(ra) + ((int32_t)readRegister32(rn) * (int32_t)readRegister32(rm));
            }
            else if (x == 1)
            {
                res = (int32_t)readRegister32(ra) - ((int32_t)readRegister32(rn) * (int32_t)readRegister32(rm));
            }
            writeRegister32(rd, res);
        }
        else if (sf == 1)
        {
            int64_t res;
            if (x == 0)
            {
                res = (int64_t)readRegister64(ra) + ((int64_t)readRegister64(rn) * (int64_t)readRegister64(rm));
            }
            else if (x == 1)
            {
                res = (int64_t)readRegister64(ra) - ((int64_t)readRegister64(rn) * (int64_t)readRegister64(rm));
            }
            writeRegister64(rd, res);
        }
        else
        {
            printf("INCORRECT SF TYPE");
            exit(44);
        }
    }
}

uint32_t ror32(uint32_t num, int shift)
{
    return (num >> shift) | (num << (32 - shift));
}

uint64_t ror64(uint64_t num, int shift)
{
    return (num >> shift) | (num << (64 - shift));
}

void dprSetLogicPSFlags(int64_t result)
{
    printf("dprSetLogicPSFlags res: %lu\n", result);
    machineState.ps.N = result < 0;  // N is set to the sign bit of the result
    machineState.ps.Z = result == 0; // Z is set only when the result is all zero
    machineState.ps.C = 0;           // Always zero as there will never be overflow or underflow
    machineState.ps.V = 0;           // Always zero as there will never be overflow or underflow
}

void dpr_logic(int opr, int operand, int rm, int opc, int rd, int rn, int sf)
{

    int shiftShiftAmount = 1;
    int shiftKeep = 2;
    int n = shiftAndKeep(opr, 0, 1);
    int shift = shiftAndKeep(opr, shiftShiftAmount, shiftKeep);

    int32_t opResult;
    uint64_t res;

    if (shift == 0)
    {
        opResult = (sf == 0) ? (readRegister32(rm) << operand) : (readRegister64(rm) << operand);
    }
    else if (shift == 1)
    {
        opResult = (sf == 0) ? (readRegister32(rm) >> operand) : (readRegister64(rm) >> operand);
    }
    else if (shift == 2)
    {
        opResult = (sf == 0) ? (((int32_t)readRegister32(rm)) >> operand) : (((int32_t)readRegister64(rm)) >> operand);
    }
    else if (shift == 3)
    {
        opResult = (sf == 0) ? (ror32(readRegister32(rm), operand)) : (ror64(readRegister32(rm), operand));
    }
    else
    {
        printf("DPR: Unknown shift code\n");
    }

    opResult = (n == 0) ? opResult : ~opResult;

    // changing sf to n
    if (sf == 0)
    {
        switch (opc)
        {
        case 0:
            res = readRegister32(rn) & opResult;
            writeRegister32(rd, res);
            break;
        case 1:
            res = readRegister32(rn) | opResult;
            writeRegister32(rd, res);
            break;
        case 2:
            res = readRegister32(rn) ^ opResult;
            writeRegister32(rd, res);
            break;
        case 3:
            res = readRegister32(rn) & opResult;
            writeRegister32(rd, res);
            dprSetLogicPSFlags((int32_t)readRegister32(rd));
            break;
        default:
            printf("UNKNOWN DPR opc\n");
        }
        // changing n to sf
    }
    else if (sf == 1)
    {
        switch (opc)
        {
        case 0:
            res = readRegister64(rn) & opResult;
            writeRegister64(rd, res);
            break;
        case 1:
            res = readRegister64(rn) | opResult;
            writeRegister64(rd, res);
            break;
        case 2:
            res = readRegister64(rn) ^ opResult;
            writeRegister64(rd, res);
            break;
        case 3:
            res = readRegister64(rn) & opResult;
            writeRegister64(rd, res);
            dprSetLogicPSFlags(readRegister64(rd));
            break;
        default:
            printf("UNKNOWN DPR opc\n");
        }
    }
    else
    {
        printf("DPR: Unknown sf\n");
    }
}

void dpr(uint32_t instruction)
{
    int rd = findRd(instruction);
    int sf = findSf(instruction);
    int rn = findRn(instruction);
    int opc = findOpc(instruction);

    int mShift = 28;
    int mKeep = 1;
    int m = shiftAndKeep(instruction, mShift, mKeep);

    int operandShift = 10;
    int operandKeep = 6;
    int operand = shiftAndKeep(instruction, operandShift, operandKeep);

    int rmShift = 16;
    int rmKeep = 5;
    int rm = shiftAndKeep(instruction, rmShift, rmKeep);

    int oprShift = 21;
    int oprKeep = 4;
    int opr = shiftAndKeep(instruction, oprShift, oprKeep);

    if (m == 1)
    {
        dpr_multiply(operand, rd, rn, rm, sf);
    }

    else if (m == 0 && opr >= 8)
    {

        int shiftShiftAmount = 1;
        int shiftKeep = 2;
        int shift = shiftAndKeep(opr, shiftShiftAmount, shiftKeep);

        uint64_t opResult;
        printf("res before : %x\n", readRegister32(rm));
        if (shift == 0)
        {
            printf("lsl by: %d\n", operand);
            opResult = (sf == 0) ? (readRegister32(rm) << operand) : (readRegister64(rm) << operand);
        }
        else if (shift == 1)
        {
            printf("lsr by: %d\n", operand);
            opResult = (sf == 0) ? (readRegister32(rm) >> operand) : (readRegister64(rm) >> operand);
        }
        else if (shift == 2)
        {
            printf("asr by: %d\n", operand);
            opResult = (sf == 0) ? (((int32_t)readRegister32(rm)) >> operand) : (((int32_t)readRegister64(rm)) >> operand);
        }
        else
        {
            printf("DPR: opResult not set");
        }

        printf("dpr: rn:%x, res:%lx\n", rn, opResult);
        if (sf == 0)
        {
            arithmetic(opc, readRegister32(rn), opResult, rd, sf);
        }
        else
        {
            arithmetic(opc, readRegister64(rn), opResult, rd, sf);
        }
    }

    else if (m == 0 && opr < 8)
    {
        dpr_logic(opr, operand, rm, opc, rd, rn, sf);
    }
    else
    {
        printf("DPR logic not called\n");
    }
};