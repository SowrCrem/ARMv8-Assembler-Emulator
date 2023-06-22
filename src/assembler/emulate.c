#include "helperFuncs.h"

#define TOT_INSTRUCT_NUM pow(2, 21)

#define GROUP_MASK 15
#define GROUP_SHIFT 25
#define NOP_INSTRUCT 0xD503201F
#define HALT_INSTRUCT 0x8a000000

uint32_t fetch()
{
    return readMemory32(machineState.pc);
}

void decodeAndExecute(uint32_t instruction)
{
    int comp = (instruction >> GROUP_SHIFT) & GROUP_MASK;
    if (comp == 8 || comp == 9)
    {
        printf("||-- This is a DPI instruction --||\n");
        dpi(instruction);
    }
    else if (comp == 5 || comp == 13)
    {
        printf("||-- This is a DPR instruction --||\n");
        dpr(instruction);
    }
    else if (comp == 10 || comp == 11)
    {
        printf("||-- This is a Branch instruction --||\n");
        branch(instruction);
    }
    else
    {
        printf("||-- This is a Load/Store instruction --||\n");
        loadorstore(instruction);
    }
    machineState.pc += 4;
    if (comp == 10 || comp == 11)
    {
        machineState.pc -= 4;
    }
}

int main(int argc, char **argv)
{
    // int used to make sure memory is instruction addressable
    byteMemory = calloc(TOT_INSTRUCT_NUM, sizeof(*byteMemory));
    assert(byteMemory != NULL);
    byteReadFile(argv[1]);
    machineState.pc = 0;
    machineState.ps.Z = true;
    uint32_t instruction = 0;
    bool halt = false;
    while (!halt)
    {
        instruction = fetch();
        printf("--------------Instruction: %08x,  PC: %ld------------ \n", instruction, machineState.pc);
        switch (instruction)
        {
        case NOP_INSTRUCT:
            machineState.pc += 4;
            break;
        case HALT_INSTRUCT:
            halt = true;
            break;
        default:
            decodeAndExecute(instruction);
        }
    }
    writeOutput(argv[2]);
    free(byteMemory);
    return EXIT_SUCCESS;
}
