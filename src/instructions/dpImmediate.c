#include "dpImmediate.h"
#include "../utils/utils.h"
#include "../utils/storage.h"

uint64_t movk(uint32_t hw, uint32_t imm16, uint32_t rd) {
    uint64_t reg = readGeneral(rd, 64);
    int start = (int) (hw * 16);
    uint64_t to_set;

    uint64_t mask1 = ~(0xFFFF << start);
    to_set = reg & mask1;
    uint64_t mask2 = imm16 << start;

    to_set = to_set | mask2;
    return to_set;
}

void dataProcessingImmediate(uint32_t instr) {
    uint32_t sf = extractBits(instr, 31, 31);
    uint32_t opc = extractBits(instr, 29, 30);
    uint32_t opi = extractBits(instr, 23, 25);
    uint32_t rd = extractBits(instr, 0, 4);
    uint32_t operand = extractBits(instr, 5, 22);

    //Arithmetic
    if (opi == 2) {
        uint32_t sh = extractBits(instr, 22, 22);
        uint32_t mode = (sf == 0 ? 32 : 64);
        uint32_t imm12 = extractBits(instr, 10, 21);
        uint32_t rn = extractBits(instr, 5, 9);
        if (sh == 1) {
            imm12 <<= 12;
        }


        //32 bit arithmetic
        if (sf == 0) {
            int32_t rn_value = (int) readGeneral(rd, 32);

            if (opc == 0) {
                int32_t to_set = (int) imm12 + rn_value;
                writeGeneral(rd, to_set, (int) mode);

                //Adding and setting flags
            } else if (opc == 1) {
                int32_t to_set = (int) imm12 + rn_value;
                writeGeneral(rd, to_set, (int) mode);

                int nValue = (to_set > 0 ? 0 : 1);
                writeN(nValue);

                int zValue = (to_set == 0 ? 0 : 1);
                writeZ(zValue);

                int vValue = ((imm12 >= 0 && rn_value >= 0 && to_set < 0) ? 1 : 0);
                writeV(vValue);

                int cValue = ((to_set < imm12 || to_set < rn_value) ? 1 : 0);
                writeC(cValue);
            } else if (opc == 2) {
                int32_t to_set = (int) (rn_value - imm12);
                writeGeneral(rd, to_set, (int) mode);

                //Subtracting and setting flags
            } else {
                int32_t to_set = (int) (rn_value - imm12);
                writeGeneral(rd, to_set, (int) mode);

                int nValue = (to_set > 0 ? 0 : 1);
                writeN(nValue);

                int zValue = (to_set == 0 ? 0 : 1);
                writeZ(zValue);

                int vValue = ((to_set > rn_value && imm12 > 0) ? 1 : 0);
                writeV(vValue);

                int cValue = ((imm12 > rn_value) ? 0 : 1);
                writeC(cValue);
            }
            //64-bit arithmetic
        } else if (sf == 1) {
            int64_t rn_value = (int) readGeneral(rd, 64);

            //Adding
            if (opc == 0) {
                int64_t to_set = imm12 + rn_value;
                writeGeneral(rd, to_set, (int) mode);

                //Adding and setting flags
            } else if (opc == 1) {
                int64_t to_set = imm12 + rn_value;
                writeGeneral(rd, to_set, (int) mode);

                int nValue = (to_set > 0 ? 0 : 1);
                writeN(nValue);

                int zValue = (to_set == 0 ? 0 : 1);
                writeZ(zValue);

                int vValue = ((imm12 >= 0 && rn_value >= 0 && to_set < 0) ? 1 : 0);
                writeV(vValue);

                int cValue = ((to_set < imm12 || to_set < rn_value) ? 1 : 0);
                writeC(cValue);
            } else if (opc == 2) {
                int64_t to_set = rn_value - imm12;
                writeGeneral(rd, to_set, (int) mode);

                //Subtracting and setting flags
            } else {
                int64_t to_set = rn_value - imm12;
                writeGeneral(rd, to_set, (int) mode);

                int nValue = (to_set > 0 ? 0 : 1);
                writeN(nValue);

                int zValue = (to_set == 0 ? 0 : 1);
                writeZ(zValue);

                int vValue = ((to_set > rn_value && imm12 > 0) ? 1 : 0);
                writeV(vValue);

                int cValue = ((imm12 > rn_value) ? 0 : 1);
                writeC(cValue);
            }
        }
    } else if (opi == 5) {

        uint32_t hw = extractBits(instr, 21, 22);
        uint32_t imm16 = extractBits(instr, 15, 20);
        uint32_t opval = imm16 << (hw * 16);

        //Move with not
        if (opc == 0) {
            if (sf == 1) {
                uint64_t to_set = ~opval;
                writeGeneral(rd, to_set, 64);

            } else {
                uint32_t to_set = (uint64_t) ~opval;
                writeGeneral(rd, to_set, 64);
            }
            //Move with zero
        } else if (opc == 2) {
            if (sf == 1) {
                writeGeneral(rd, (uint64_t) opval, 64);
            } else {
                writeGeneral(rd, opval, 32);
            }

            //Move with keep
        } else if (opc == 3) {
            uint64_t to_set = movk(hw, imm16, rd);
            if (sf == 0) {
                uint64_t mask = 0xFFFFFFFF; // when only 32 bits
                to_set = to_set & mask;
            }
            writeGeneral(rd, to_set, 64);
        }

    }


}

