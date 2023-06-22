#include "helperFuncs.h"

#define sfShift 30
#define sfKeep 1

void sdt(uint32_t instruction)
{

    uint32_t address;
    uint32_t sf = shiftAndKeep(instruction, sfShift, sfKeep);
    bool mode = sf == 0;

    int LShift = 22;
    int LKeep = 1;
    uint32_t L = shiftAndKeep(instruction, LShift, LKeep);

    int UShift = 24;
    int UKeep = 1;
    uint32_t U = shiftAndKeep(instruction, UShift, UKeep);

    int offsetShift = 10;
    int offsetKeep = 12;
    uint32_t offset = shiftAndKeep(instruction, offsetShift, offsetKeep);

    int xnShift = 5;
    int xnKeep = 5;
    uint32_t xn = shiftAndKeep(instruction, xnShift, xnKeep);

    int rtShift = 0;
    int rtKeep = 5;
    uint32_t rt = shiftAndKeep(instruction, rtShift, rtKeep);

    // Unsigned Immediate Offset mode
    if (U == 1)
    {
        address = readRegister64(xn) + (uint64_t)offset;

        // Load operation
        if (L == 1)
        {

            // 32 bit
            if (mode)
            {
                printf("32 bit unsigned immediate offset load operation... ");
                writeRegister32(rt, readMemory32(address));
            }
            else
            {
                printf("64 bit unsigned immediate offset load operation... ");
                uint64_t val = readMemory64(address);
                printf("Storing value %lu from memory %016d\n", val, address);
                writeRegister64(rt, val);
            }
            // machineState.general[rt] = readMemory(address);
        }

        // Store operation
        else
        {

            // 32 bit mode
            if (mode)
            {
                printf("32 bit unsigned immediate offset store operation... ");
                setMemory32(address, readRegister32(rt));
            }

            // 64 bit mode
            else
            {
                printf("64 bit unsigned immediate offset store operation... ");
                setMemory64(address, readRegister64(rt));
            }

            // setMemory(address, machineState.general[rt]);
        }
    }
    else
    {

        uint32_t registerOrIndex = shiftAndKeep(instruction, 21, 1);

        // Register Offset addressing mode
        if (registerOrIndex == 1)
        {
            int xmShift = 16;
            int xmKeep = 5;
            uint32_t xm = shiftAndKeep(instruction, xmShift, xmKeep);

            address = readRegister64(xn) + readRegister64(xm);

            // Load operation
            if (L == 1)
            {

                // 32 bit mode
                if (sf == 0)
                {
                    printf("32 bit register offset load operation... ");
                    writeRegister32(rt, readMemory32(address));
                }

                // 64 bit mode
                else
                {
                    printf("64 bit register offset load operation... ");
                    writeRegister64(rt, readMemory64(address));
                }

                // writeRegister(rt, mode, readMemory(address));
            }

            // Store operation
            else
            {

                if (mode)
                {
                    printf("32 bit register offset store operation... ");
                    setMemory32(address, readRegister32(rt));
                }
                else
                {
                    printf("64 bit register offset store operation... ");
                    setMemory64(address, readRegister64(rt));
                }
                // setMemory(address, machineState.general[rt]);
            }
        }

        // Pre/Post Indexed
        else
        {
            int iShift = 11;
            int iKeep = 1;
            uint32_t i = shiftAndKeep(instruction, iShift, iKeep);

            int simmShift = 12;
            int simmKeep = 9;
            uint32_t simm = shiftAndKeep(instruction, simmShift, simmKeep);

            // Pre-Indexed addressing mode
            if (i == 1)
            {
                address = readRegister64(xn) + (int64_t)simm;
                writeRegister64(xn, address);
                // Load operation
                if (L == 1)
                {
                    if (mode)
                    {
                        printf("32 bit pre-indexed load operation... ");
                        writeRegister32(rt, readMemory32(address));
                    }
                    else
                    {
                        printf("64 bit pre-indexed load operation... ");
                        writeRegister64(rt, readMemory64(address));
                    }
                    // machineState.general[rt] = readMemory(address);
                }

                // Store operation
                else
                {

                    if (mode)
                    {
                        printf("32 bit pre-indexed store operation... ");
                        setMemory32(address, readRegister32(rt));
                    }
                    else
                    {
                        printf("64 bit pre-indexed store operation... ");
                        setMemory64(address, readRegister64(rt));
                    }
                    // setMemory(address, machineState.general[rt]);
                }

                // machineState.general[xn] = address;
            }

            // Post-Indexed addressing mode
            else
            {
                address = readRegister64(xn);

                // Load operation
                if (L == 1)
                {
                    if (mode)
                    {
                        printf("32 bit post-indexed load operation...\n ");
                        writeRegister32(rt, readMemory32(address));
                    }
                    else
                    {
                        printf("64 bit post-indexed load operation...\n ");
                        writeRegister64(rt, readMemory64(address));
                    }

                    // machineState.general[rt] = readMemory(address);
                }

                // Store operation
                else
                {

                    if (mode)
                    {
                        printf("32 bit post-indexed store operation...\n ");
                        setMemory32(address, readRegister32(rt));
                    }
                    else
                    {
                        printf("64 bit post-indexed store operation...\n ");
                        setMemory64(address, readRegister64(rt));
                    }
                    // setMemory(address, machineState.general[rt]);
                }
                writeRegister64(xn, address + (int64_t)(int32_t)simm);
                // machineState.general[xn] += simm;
            }
        }
    }
};

void ll(uint32_t instruction)
{

    uint32_t sf = shiftAndKeep(instruction, sfShift, sfKeep);
    bool mode = sf == 0;

    int rtShift = 0;
    int rtKeep = 5;
    uint32_t rt = shiftAndKeep(instruction, rtShift, rtKeep);

    int simmShift = 5;
    int simmKeep = 19;
    int64_t simm = shiftAndKeepSigned(instruction, simmShift, simmKeep);

    int address = (int64_t)machineState.pc + simm * 4;

    if (mode)
    {
        printf("32 bit load literal operation from memory address %d into W%d... ", address, rt);
        writeRegister32(rt, readMemory32(address));
    }

    else
    {
        printf("64 bit load literal operation from memory address %d into X%d... ", address, rt);
        writeRegister64(rt, readMemory64(address));
    }
    // writeRegister(rt, readMemory(address));
};

void loadorstore(uint32_t instruction)
{
    printf("Deciding load or store... ");
    uint32_t comp = shiftAndKeep(instruction, 31, 1);
    if (comp == 1)
    {
        sdt(instruction);
    }
    else
    {
        ll(instruction);
    }
}
