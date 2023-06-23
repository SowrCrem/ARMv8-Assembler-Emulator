#include "encode.h"
#include <assert.h>
#include <stdbool.h>


uint32_t encode_op(op_t op) {
    switch (op) {
        case OP_ADD:
        case OP_BIC:
        case OP_AND:
        case OP_MADD:
        case OP_STR:
        case OP_MOVN:
            return 0b00;
        case OP_ADDS:
        case OP_ORN:
        case OP_ORR:
        case OP_MSUB:
        case OP_LDR:
            return 0b01;
        case OP_SUB:
        case OP_EON:
        case OP_EOR:
        case OP_MOVZ:
            return 0b10;
        case OP_SUBS:
        case OP_BICS:
        case OP_ANDS:
        case OP_MOVK:
            return 0b11;
        default:
            assert(false);
    }
}

uint32_t encode_arii(inst_t *inst) {

    uint32_t op = encode_op(inst->op);
    uint32_t sf = (inst->arii.sf ? 1 : 0);
    uint32_t sh_imm = inst->arii.sh.imm / 12;
    uint32_t imm = inst->arii.imm;
    uint32_t rn = inst->arii.rn;
    uint32_t rd = inst->arii.rd;

    uint32_t code = ARII_BITS | (sf << 31) | (op << 29) | (sh_imm << 22) |
                    (imm << 10) | (rn << 5) | rd;
    return code;
};

uint32_t encode_arir(inst_t *inst) {
    uint32_t op = encode_op(inst->op);
    uint32_t sf = (inst->arir.sf ? 1 : 0);
    uint32_t sh_type = inst->arir.sh.type;
    uint32_t sh_imm = inst->arir.sh.imm;
    uint32_t rm = inst->arir.rm;
    uint32_t rn = inst->arir.rn;
    uint32_t rd = inst->arir.rd;

    uint32_t code = ARIR_BITS | (sf << 31) | (op << 29) | (sh_type << 22) |
                    (rm << 16) | (sh_imm << 10) | (rn << 5) | rd;
    return code;
};

uint32_t encode_wdmv(inst_t *inst) {
    uint32_t op = encode_op(inst->op);
    uint32_t sf = (inst->wdmv.sf ? 1 : 0);
    uint32_t sh_imm = inst->wdmv.sh.imm / 16;
    uint32_t imm = inst->wdmv.imm;
    uint32_t rd = inst->wdmv.rd;

    uint32_t code =
            WDMV_BITS | (sf << 31) | (op << 29) | (sh_imm << 21) | (imm << 5) | rd;
    return code;
};

uint32_t encode_logi(inst_t *inst) {
    uint32_t op = encode_op(inst->op);
    uint32_t sf = (inst->logi.sf ? 1 : 0);
    uint32_t sh_type = inst->logi.sh.type;
    uint32_t sh_imm = inst->logi.sh.imm;
    uint32_t rm = inst->logi.rm;
    uint32_t rn = inst->logi.rn;
    uint32_t rd = inst->logi.rd;
    uint32_t neg = ((inst->op == OP_BIC || inst->op == OP_ORN ||
                     inst->op == OP_EON || inst->op == OP_BICS)
                    ? 1
                    : 0);

    uint32_t code = LOGI_BITS | (sf << 31) | (op << 29) | (sh_type << 22) |
                    (neg << 21) | (rm << 16) | (sh_imm << 10) | (rn << 5) | rd;
    return code;
};

uint32_t encode_mult(inst_t *inst) {
    uint32_t op = encode_op(inst->op);
    uint32_t sf = (inst->mult.sf ? 1 : 0);
    uint32_t rm = inst->mult.rm;
    uint32_t rn = inst->mult.rn;
    uint32_t rd = inst->mult.rd;
    uint32_t ra = inst->mult.ra;

    uint32_t code = MULT_BITS | (sf << 31) | (rm << 16) | (op << 15) |
                    (ra << 10) | (rn << 5) | rd;

    return code;
};

uint32_t set_bits(uint32_t old, int hi, int lo, uint32_t val) {
    uint32_t mask = (1 << (hi - lo + 1)) - 1;
    return ((val & mask) << lo) | (old & (~(mask << lo)));
}

uint32_t encode_bunc(inst_t *inst) {
    uint32_t offset = (uint32_t)(inst->bunc.offset / 4);

    uint32_t code = BUNC_BITS;
    code = set_bits(BUNC_BITS, 25, 0, offset);

    return code;
};

uint32_t encode_bcnd(inst_t *inst) {
    uint32_t offset = (uint32_t)(inst->bcnd.offset / 4);
    uint32_t cond = inst->bcnd.cond;

    uint32_t code = BCND_BITS | cond;
    code = set_bits(code, 23, 5, offset);

    return code;
};

uint32_t encode_breg(inst_t *inst) {
    uint32_t xn = (uint32_t)(inst->breg.xn);

    uint32_t code = BREG_BITS | (xn << 5);

    return code;
};

uint32_t encode_sdti(inst_t *inst) {
    uint32_t sf = (inst->sdti.sf ? 1 : 0);
    uint32_t op = encode_op(inst->op);
    uint32_t simm = (uint32_t)(inst->sdti.simm);
    uint32_t pre = (inst->sdti.pre ? 1 : 0);
    uint32_t xn = inst->sdti.xn;
    uint32_t rt = inst->sdti.rt;

    uint32_t code =
            SDTI_BITS | (sf << 30) | (op << 22) | (pre << 11) | (xn << 5) | rt;
    code = set_bits(code, 20, 12, simm);

    return code;
};

uint32_t encode_sdtu(inst_t *inst) {
    uint32_t sf = (inst->sdtu.sf ? 1 : 0);
    uint32_t op = encode_op(inst->op);
    uint32_t imm = inst->sdtu.imm / (sf ? 8 : 4);
    uint32_t xn = inst->sdtu.xn;
    uint32_t rt = inst->sdtu.rt;

    uint32_t code =
            SDTU_BITS | (sf << 30) | (op << 22) | (imm << 10) | (xn << 5) | rt;

    return code;
};

uint32_t encode_sdtr(inst_t *inst) {
    uint32_t sf = (inst->sdtr.sf ? 1 : 0);
    uint32_t op = encode_op(inst->op);
    uint32_t xm = inst->sdtr.xm;
    uint32_t xn = inst->sdtr.xn;
    uint32_t rt = inst->sdtr.rt;

    uint32_t code =
            SDTR_BITS | (sf << 30) | (op << 22) | (xm << 16) | (xn << 5) | rt;

    return code;
};

uint32_t encode_ldlt(inst_t *inst) {
    uint32_t sf = (inst->ldlt.sf ? 1 : 0);
    uint32_t literal = (uint32_t)(inst->ldlt.literal / 4);
    uint32_t rt = inst->ldlt.rt;

    uint32_t code = LDLT_BITS | (sf << 30) | rt;
    code = set_bits(code, 23, 5, literal);

    return code;
};

uint32_t encode_spec(inst_t *inst) {
    uint32_t code;
    switch (inst->op) {
        case OP_NOP:
            code = NOP__BITS;
            break;
        case OP_HALT:
            code = HALT_BITS;
            break;
        case OP_INT:
            code = inst->data.data;
            break;
        default:
            assert(false);
    }

    return code;
};

uint32_t encode(inst_t *inst) {
    if (inst->type == INST_ARII) {
        return encode_arii(inst);
    } else if (inst->type == INST_ARIR) {
        return encode_arir(inst);
    } else if (inst->type == INST_WDMV) {
        return encode_wdmv(inst);
    } else if (inst->type == INST_LOGI) {
        return encode_logi(inst);
    } else if (inst->type == INST_MULT) {
        return encode_mult(inst);
    } else if (inst->type == INST_BUNC) {
        return encode_bunc(inst);
    } else if (inst->type == INST_BCND) {
        return encode_bcnd(inst);
    } else if (inst->type == INST_BREG) {
        return encode_breg(inst);
    } else if (inst->type == INST_SDTI) {
        return encode_sdti(inst);
    } else if (inst->type == INST_SDTU) {
        return encode_sdtu(inst);
    } else if (inst->type == INST_SDTR) {
        return encode_sdtr(inst);
    } else if (inst->type == INST_LDLT) {
        return encode_ldlt(inst);
    } else if (inst->type == INST_SPEC) {
        return encode_spec(inst);
    } else {
        fprintf(stderr, "unknown inst->type: %d\n", inst->type);
        assert(false);
    }
}