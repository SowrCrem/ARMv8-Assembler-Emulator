#include "dpRegister.h"
#include "../utils/utils.h"
#include "../utils/storage.h"
#include <stdbool.h>

// BITWISE SHIFT FUNCTIONS

// Logical Shift Left (LSL) for uint64_t
uint64_t lsl64(uint64_t value, uint32_t shift) {
    return value << shift;
}

// Logical Shift Left (LSL) for uint32_t
uint32_t lsl32(uint32_t value, uint32_t shift) {
    return value << shift;
}

// Logical Shift Right (LSR) for uint64_t
uint64_t lsr64(uint64_t value, uint32_t shift) {
    return value >> shift;
}

// Logical Shift Right (LSR) for uint32_t
uint32_t lsr32(uint32_t value, uint32_t shift) {
    return value >> shift;
}

// Arithmetic Shift Right (ASR) for int64_t
int64_t asr64(int64_t value, uint32_t shift) {
    int64_t sign_bit = value < 0 ? (int64_t) 1 << 63 : 0;    // Extract sign bit
    int64_t shifted_value = value >> shift;                 // Perform the shift
    int64_t sign_extension = sign_bit >> shift;             // Extend the sign bit
    return shifted_value | sign_extension;                  // Combine the shifted value and sign extension
}

// Arithmetic Shift Right (ASR) for int32_t
int32_t asr32(int32_t value, uint32_t shift) {
    int32_t sign_bit = value < 0 ? (int32_t) 1 << 31 : 0;    // Extract sign bit
    int32_t shifted_value = value >> shift;                 // Perform the shift
    int32_t sign_extension = sign_bit >> shift;             // Extend the sign bit
    return shifted_value | sign_extension;                  // Combine the shifted value and sign extension
}

// Rotate Right (ROR) for uint64_t
uint64_t ror64(uint64_t value, uint32_t shift) {
    uint32_t num_bits = sizeof(uint64_t) * 8;
    shift %= num_bits;
    return (value >> shift) | (value << (num_bits - shift));
}

// Rotate Right (ROR) for uint32_t
uint32_t ror32(uint32_t value, uint32_t shift) {
    uint32_t num_bits = sizeof(uint32_t) * 8;
    shift %= num_bits;
    return (value >> shift) | (value << (num_bits - shift));
}

/****************************************************************************************************************/


void arithmetic(uint32_t instruction) {
    int32_t sf = getBit(instruction, 31);
    int32_t opc = extractBits(instruction, 29,30);
    int32_t opr = extractBits(instruction, 21,24);
    int32_t rm = extractBits(instruction, 16, 20);
    uint32_t operand = extractBits(instruction, 10, 15);
    int32_t rn = extractBits(instruction,5,9);
    uint32_t rd = extractBits(instruction, 0, 4);
    int32_t shift = extractBits(instruction, 22, 23);
    int32_t n = getBit(instruction, 21);

    if (n==0) {
        if (sf == 0) {
            int32_t op2;
            switch (shift) {
                case 0:
                    op2 = lsl32(readGeneral(rm, 32), operand);
                    break;
                case 1:
                    op2 = lsr32(readGeneral(rm, 32), operand);
                    break;
                case 2:
                    op2 = asr32(readGeneral(rm, 32),operand);
                    break;
            }
            int32_t to_write;
            int32_t rn_value = readGeneral(rn, 32);
            switch (opc) {
                case 0:
                    to_write = op2 & rn_value;
                    writeGeneral(rd, to_write, 32);
                    break;
                case 1:
                    to_write = rn_value | op2;
                    writeGeneral(rd, to_write, 32);
                    break;
                case 2:
                    to_write = rn_value ^ op2;
                    writeGeneral(rd, to_write, 32);
                    break;
                case 3:
                    to_write = rn_value & op2;
                    writeGeneral(rd, to_write, 32);
                    if (to_write < 0) {
                        writeN(1);
                    } else {
                        writeN(0);
                    }
                    if (to_write == 0) {
                        writeZ(1);
                    } else {
                        writeZ(0);
                    }
                    writeC(0);
                    writeV(0);
                    break;
            }
        } else if (sf==1) {
            int64_t op2;
            switch (shift) {
                case 0:
                    op2 = lsl64(readGeneral(rm, 64), operand);
                    break;
                case 1:
                    op2 = lsr64(readGeneral(rm, 64), operand);
                    break;
                case 2:
                    op2 = asr64(readGeneral(rm, 64),operand);
                    break;
            }
            int64_t to_write;
            int64_t rn_value = readGeneral(rn, 64);
            switch (opc) {
                case 0:
                    to_write = op2 & rn_value;
                    writeGeneral(rd, to_write, 64);
                case 1:
                    to_write = rn_value | op2;
                    writeGeneral(rd, to_write, 64);
                case 2:
                    to_write = rn_value ^ op2;
                    writeGeneral(rd, to_write, 64);
                case 3:
                    to_write = rn_value & op2;
                    writeGeneral(rd, to_write, 64);
                    if (to_write < 0) {
                        writeN(1);
                    } else {
                        writeN(0);
                    }
                    if (to_write == 0) {
                        writeZ(1);
                    } else {
                        writeZ(0);
                    }
                    writeC(0);
                    writeV(0);
            }
        }
    } else {
        if (sf == 0) {
            int32_t op2;
            switch (shift) {
                case 0:
                    op2 = lsl32(readGeneral(rm, 32), operand);
                    break;
                case 1:
                    op2 = lsr32(readGeneral(rm, 32), operand);
                    break;
                case 2:
                    op2 = asr32(readGeneral(rm, 32),operand);
                    break;
                case 3:
                    op2 = ror32(readGeneral(rm, 32), operand);
            }
            int32_t to_write;
            int32_t rn_value = readGeneral(rn, 32);
            switch (opc) {
                case 0:
                    to_write = (~op2) & rn_value;
                    writeGeneral(rd, to_write, 32);
                    break;
                case 1:
                    to_write = rn_value | ~op2;
                    writeGeneral(rd, to_write, 32);
                    break;
                case 2:
                    to_write = rn_value ^ ~op2;
                    writeGeneral(rd, to_write, 32);
                    break;
                case 3:
                    to_write = rn_value & ~op2;
                    writeGeneral(rd, to_write, 32);
                    if (to_write < 0) {
                        writeN(1);
                    } else {
                        writeN(0);
                    }
                    if (to_write == 0) {
                        writeZ(1);
                    } else {
                        writeZ(0);
                    }
                    writeC(0);
                    writeV(0);
                    break;
            }
        } else if (sf == 1) {
            int64_t op2;
            switch (shift) {
                case 0:
                    op2 = lsl64(readGeneral(rm, 64), operand);
                    break;
                case 1:
                    op2 = lsr64(readGeneral(rm, 64), operand);
                    break;
                case 2:
                    op2 = asr64(readGeneral(rm, 64),operand);
                    break;
            }
            int64_t to_write;
            int64_t rn_value = readGeneral(rn, 64);
            switch (opc) {
                case 0:
                    to_write = ~op2 & rn_value;
                    writeGeneral(rd, to_write, 64);
                case 1:
                    to_write = rn_value | ~op2;
                    writeGeneral(rd, to_write, 64);
                case 2:
                    to_write = rn_value ^ ~op2;
                    writeGeneral(rd, to_write, 64);
                case 3:
                    to_write = rn_value & ~op2;
                    writeGeneral(rd, to_write, 64);
                    if (to_write < 0) {
                        writeN(1);
                    } else {
                        writeN(0);
                    }
                    if (to_write == 0) {
                        writeZ(1);
                    } else {
                        writeZ(0);
                    }
                    writeC(0);
                    writeV(0);
            }
        }
    }
}



void multiply(uint32_t instruction) {

    int32_t sf = getBit(instruction, 31);
    int32_t x = getBit(instruction, 15);
    int32_t ra = extractBits(instruction, 10, 14);
    int32_t rm = extractBits(instruction, 16,20);
    int32_t rn = extractBits(instruction, 5,9);
    int32_t rd = extractBits(instruction, 0,4);


    if (ra == 31) {
        writeZ(1);
    } else {
        if (x == 0) {
            if (sf == 1) {
                int64_t data = readMemory(ra) + (readMemory(rn) * readMemory(rm));
                writeGeneral(rd, data, 64);
            } else if (sf == 0) {
                int32_t data = readMemory(ra) + (readMemory(rn) * readMemory(rm));
                writeGeneral(rd, data, 32);
            }
        } else if (x==1) {
            if (sf == 1) {
                int64_t data = readMemory(ra) - (readMemory(rn)* readMemory(rm));
                writeGeneral(rd, data, 32);
            } else if (sf == 0) {
                int32_t data = readMemory(ra) - (readMemory(rn) * readMemory(rm));
                writeGeneral(rd, data, 32);
            }
        }
    }


}

void dataProcessingRegister(uint32_t instruction) {
    bool m1 = getMSB(instruction << 3);
    if (m1 == 1) {
        multiply(instruction);
    } else {  // m1 = 0;
        arithmetic(instruction);
    }
}