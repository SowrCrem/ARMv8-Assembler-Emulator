#include "helperFuncs.h"

void branch(uint32_t instruction)
{

    int brType = shiftAndKeep(instruction, 30, 2);

    enum branchType
    {
        UNCONDITIONAL = 0,
        CONDITIONAL = 1,
        REGISTER = 3
    };

    if (brType == UNCONDITIONAL)
    {
        int simm26Shift = 0;
        int simm26Keep = 26;
        int simm26 = shiftAndKeep(instruction, simm26Shift, simm26Keep);
        int64_t offset = signExtend(simm26, simm26Keep) * 4;
        machineState.pc = ((int64_t)machineState.pc) + offset;
    }
    else if (brType == REGISTER)
    {
        int xnShift = 5;
        int xnKeep = 5;
        int xn = shiftAndKeep(instruction, xnShift, xnKeep);
        machineState.pc = machineState.general[xn];
    }
    else if (brType == CONDITIONAL)
    {

        enum cond
        {
            EQ = 0b00000,
            NE = 0b00001,
            GE = 0b01010,
            LT = 0b01011,
            GT = 0b01100,
            LE = 0b01101,
            AL = 0b01110
        };
        int condShift = 0;
        int condKeep = 4;
        enum cond condition = shiftAndKeep(instruction, condShift, condKeep);

        PSTATE ps = machineState.ps;
        bool conditionMet = false;
        switch (condition)
        {
        case EQ:
            if (ps.Z == 1)
                conditionMet = true;
            break;
        case NE:
            if (ps.Z == 0)
                conditionMet = true;
            break;
        case GE:
            if (ps.N == 1)
                conditionMet = true;
            break;
        case LT:
            if (ps.N != 1)
                conditionMet = true;
            break;
        case GT:
            if (ps.Z == 0 && ps.N == ps.V)
                conditionMet = true;
            break;
        case LE:
            if (!(ps.Z == 0 && ps.N == ps.V))
                conditionMet = true;
            break;
        case AL:
            conditionMet = true;
            break;
        default:
            printf("UNKNOWN BRANCH CONDITION");
            exit(41);
        }

        if (conditionMet)
        {
            int simm19Shift = 5;
            int simm19Keep = 19;
            int simm19 = shiftAndKeep(instruction, simm19Shift, simm19Keep);
            int64_t offset = signExtend(simm19, simm19Keep) * 4;
            machineState.pc = ((int64_t)machineState.pc) + offset;
        }
        else
        {
            machineState.pc += 4;
        }
    }
    else
    {
        printf("UNKNOWN BRANCH INSTRUCTION");
        exit(42);
    }
}