#include "parse.h"

//
// mnemonics' parsers
//
static int parse_addr(int32_t *offset, char *f_lit, context_t *ctx) {
    int32_t addr;
    if (f_lit[0] == '#') {
        addr = atoi(f_lit + 1);
    } else {
        addr = lookup_symbol(ctx, f_lit);
    }

    *offset = addr - ctx->pc;
    return 0;
}

static int parse_number(long *num, char *s) {
    int base = 10;
    if (strlen(s) > 2 && s[1] == 'x') {
        s += 2;
        base = 16;
    }
    char *endptr;
    *num = strtol(s, &endptr, base);
    return 0;
}

static int parse_reg(uint8_t *r, char *reg, bool *sf) {
    if (sf) {
        *sf = (reg[0] == 'x');
    }

    if (strcmp(reg + 1, "zr") == 0) {
        *r = 31;
    } else {
        *r = atoi(reg + 1);
    }

    return 0;
}

static int parse_imm(uint32_t *u32, char *imm) {
    long num;
    int ret = parse_number(&num, imm + 1);
    if (ret == 0) {
        *u32 = (uint32_t)num;
    }

    return ret;
}

static int parse_simm(int32_t *i32, char *imm) {
    long num;
    int ret = parse_number(&num, imm + 1);
    if (ret == 0) {
        *i32 = (int32_t)num;
    }

    return ret;
}

static int parse_imm_u16(uint16_t *u16, char *imm) {
    uint32_t u32;
    int ret = parse_imm(&u32, imm);
    if (ret == 0) {
        *u16 = (uint16_t)u32;
    }
    return ret;
}

static int parse_imm_u8(uint8_t *u8, char *imm) {
    uint32_t u32;
    int ret = parse_imm(&u32, imm);
    if (ret == 0) {
        *u8 = (uint8_t)u32;
    }
    return ret;
}

static int parse_shift(shift_t *sh, char *sh_type, char *sh_imm) {
    if (sh_type == NULL || sh_imm == NULL) {
        sh->type = SH_LSL;
        sh->imm = 0;
        return 0;
    }

    if (strcmp(sh_type, "lsl") == 0) {
        sh->type = SH_LSL;
    } else if (strcmp(sh_type, "lsr") == 0) {
        sh->type = SH_LSR;
    } else if (strcmp(sh_type, "asr") == 0) {
        sh->type = SH_ASR;
    } else if (strcmp(sh_type, "ror") == 0) {
        sh->type = SH_ROR;
    } else {
        sh->type = SH_LSL;
        sh->imm = 0;
        return 0;
    }

    return parse_imm_u8(&sh->imm, sh_imm);
}

static int parse_add_sub(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    char *f_rd = fields->fields[1];
    char *f_rn = fields->fields[2];
    char *f_opr = fields->fields[3];
    char *f_sh_type = (fields->count == 6 ? fields->fields[4] : NULL);
    char *f_sh_imm = (fields->count == 6 ? fields->fields[5] : NULL);
    if (f_opr[0] == '#') {
        inst->type = INST_ARII;

        if (ret == 0) {
            ret = parse_reg(&inst->arii.rd, f_rd, &inst->arii.sf);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->arii.rn, f_rn, NULL);
        }
        if (ret == 0) {
            ret = parse_imm_u16(&inst->arii.imm, f_opr);
        }
        if (ret == 0) {
            ret = parse_shift(&inst->arii.sh, f_sh_type, f_sh_imm);
        }
    } else {
        inst->type = INST_ARIR;
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rd, f_rd, &inst->arir.sf);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rn, f_rn, NULL);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rm, f_opr, NULL);
        }
        if (ret == 0) {
            ret = parse_shift(&inst->arir.sh, f_sh_type, f_sh_imm);
        }
    }

    return ret;
}

static int parse_cmp(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    char *f_rn = fields->fields[1];
    char *f_opr = fields->fields[2];
    char *f_sh_type = (fields->count == 5 ? fields->fields[3] : NULL);
    char *f_sh_imm = (fields->count == 5 ? fields->fields[4] : NULL);
    if (f_opr[0] == '#') {
        inst->type = INST_ARII;

        inst->arii.rd = RZR;
        if (ret == 0) {
            ret = parse_reg(&inst->arii.rn, f_rn, &inst->arii.sf);
        }
        if (ret == 0) {
            ret = parse_imm_u16(&inst->arii.imm, f_opr);
        }
        if (ret == 0) {
            ret = parse_shift(&inst->arii.sh, f_sh_type, f_sh_imm);
        }
    } else {
        inst->type = INST_ARIR;
        inst->arir.rd = RZR;
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rn, f_rn, &inst->arii.sf);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rm, f_opr, NULL);
        }
        if (ret == 0) {
            ret = parse_shift(&inst->arir.sh, f_sh_type, f_sh_imm);
        }
    }

    return ret;
}

static int parse_neg(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    char *f_rd = fields->fields[1];
    char *f_opr = fields->fields[2];
    char *f_sh_type = (fields->count == 5 ? fields->fields[3] : NULL);
    char *f_sh_imm = (fields->count == 5 ? fields->fields[4] : NULL);
    if (f_opr[0] == '#') {
        inst->type = INST_ARII;
        if (ret == 0) {
            ret = parse_reg(&inst->arii.rd, f_rd, &inst->arii.sf);
        }
        inst->arii.rn = RZR;
        if (ret == 0) {
            ret = parse_imm_u16(&inst->arii.imm, f_opr);
        }
        if (ret == 0) {
            ret = parse_shift(&inst->arii.sh, f_sh_type, f_sh_imm);
        }
    } else {
        inst->type = INST_ARIR;
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rd, f_rd, &inst->arir.sf);
        }
        inst->arir.rn = RZR;
        if (ret == 0) {
            ret = parse_reg(&inst->arir.rm, f_opr, NULL);
        }
        if (ret == 0) {
            ret = parse_shift(&inst->arir.sh, f_sh_type, f_sh_imm);
        }
    }

    return ret;
}

static int parse_and_bic_or(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    inst->type = INST_LOGI;

    char *f_rd = fields->fields[1];
    char *f_rn = fields->fields[2];
    char *f_rm = fields->fields[3];
    char *f_sh_type = (fields->count == 6 ? fields->fields[4] : NULL);
    char *f_sh_imm = (fields->count == 6 ? fields->fields[5] : NULL);
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rd, f_rd, &inst->logi.sf);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rn, f_rn, NULL);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rm, f_rm, NULL);
    }
    if (ret == 0) {
        ret = parse_shift(&inst->logi.sh, f_sh_type, f_sh_imm);
    }

    return ret;
}

static int parse_tst(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    inst->type = INST_LOGI;

    char *f_rn = fields->fields[1];
    char *f_rm = fields->fields[2];
    char *f_sh_type = (fields->count == 5 ? fields->fields[3] : NULL);
    char *f_sh_imm = (fields->count == 5 ? fields->fields[4] : NULL);

    inst->arir.rd = RZR;
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rn, f_rn, &inst->logi.sf);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rm, f_rm, NULL);
    }
    if (ret == 0) {
        ret = parse_shift(&inst->logi.sh, f_sh_type, f_sh_imm);
    }

    return ret;
}

static int parse_movknz(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    inst->type = INST_WDMV;

    char *f_rd = fields->fields[1];
    char *f_imm = fields->fields[2];
    char *f_sh_type = (fields->count == 5 ? fields->fields[3] : NULL);
    char *f_sh_imm = (fields->count == 5 ? fields->fields[4] : NULL);

    if (ret == 0) {
        ret = parse_reg(&inst->wdmv.rd, f_rd, &inst->wdmv.sf);
    }
    if (ret == 0) {
        ret = parse_imm_u16(&inst->wdmv.imm, f_imm);
    }
    if (ret == 0) {
        ret = parse_shift(&inst->wdmv.sh, f_sh_type, f_sh_imm);
    }

    return ret;
}

static int parse_mov(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;

    inst->type = INST_LOGI;

    char *f_rd = fields->fields[1];
    char *f_rm = fields->fields[2];
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rd, f_rd, &inst->logi.sf);
    }
    inst->logi.rn = RZR;
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rm, f_rm, NULL);
    }
    if (ret == 0) {
        ret = parse_shift(&inst->logi.sh, NULL, NULL);
    }

    return ret;
}

static int parse_mvn(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;
    inst->type = INST_LOGI;

    char *f_rd = fields->fields[1];
    char *f_rm = fields->fields[2];
    char *f_sh_type = (fields->count == 5 ? fields->fields[3] : NULL);
    char *f_sh_imm = (fields->count == 5 ? fields->fields[4] : NULL);
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rd, f_rd, &inst->logi.sf);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->logi.rm, f_rm, NULL);
    }
    inst->logi.rn = RZR;
    if (ret == 0) {
        ret = parse_shift(&inst->logi.sh, f_sh_type, f_sh_imm);
    }

    return ret;
}

static int parse_madd_msub(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;
    inst->type = INST_MULT;

    char *f_rd = fields->fields[1];
    char *f_rn = fields->fields[2];
    char *f_rm = fields->fields[3];
    char *f_ra = fields->fields[4];
    if (ret == 0) {
        ret = parse_reg(&inst->mult.rd, f_rd, &inst->mult.sf);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->mult.rn, f_rn, NULL);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->mult.rm, f_rm, NULL);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->mult.ra, f_ra, NULL);
    }

    return ret;
}

static int parse_mul_mneg(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;
    inst->type = INST_MULT;

    char *f_rd = fields->fields[1];
    char *f_rn = fields->fields[2];
    char *f_rm = fields->fields[3];
    if (ret == 0) {
        ret = parse_reg(&inst->mult.rd, f_rd, &inst->mult.sf);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->mult.rn, f_rn, NULL);
    }
    if (ret == 0) {
        ret = parse_reg(&inst->mult.rm, f_rm, NULL);
    }
    inst->mult.ra = RZR;

    return ret;
}

static int parse_b(fields_t *fields, inst_t *inst, context_t *ctx) {
    int ret = 0;
    inst->type = INST_BUNC;

    char *f_literal = fields->fields[1];
    if (ret == 0) {
        ret = parse_addr(&inst->bunc.offset, f_literal, ctx);
    }
    return ret;
}

static int parse_bcond(fields_t *fields, inst_t *inst, context_t *ctx) {
    int ret = 0;
    inst->type = INST_BCND;

    char *f_literal = fields->fields[1];
    if (ret == 0) {
        ret = parse_addr(&inst->bcnd.offset, f_literal, ctx);
    }
    char *mnem = fields->fields[0];
    if (strcmp(mnem, "b.ne") == 0) {
        inst->bcnd.cond = COND_NE;
    } else if (strcmp(mnem, "b.eq") == 0) {
        inst->bcnd.cond = COND_EQ;
    } else if (strcmp(mnem, "b.lt") == 0) {
        inst->bcnd.cond = COND_LT;
    } else if (strcmp(mnem, "b.le") == 0) {
        inst->bcnd.cond = COND_LE;
    } else if (strcmp(mnem, "b.ge") == 0) {
        inst->bcnd.cond = COND_GE;
    } else if (strcmp(mnem, "b.gt") == 0) {
        inst->bcnd.cond = COND_GT;
    } else if (strcmp(mnem, "b.al") == 0) {
        inst->bcnd.cond = COND_AL;
    } else {
        assert(false);
    }
    return ret;
}

static int parse_br(fields_t *fields, inst_t *inst, context_t *ctx) {
    (void)ctx;
    int ret = 0;
    inst->type = INST_BREG;
    char *f_xn = fields->fields[1];
    if (ret == 0) {
        ret = parse_reg(&inst->breg.xn, f_xn, NULL);
    }
    return ret;
}

static int parse_sdr_ldr(fields_t *fields, inst_t *inst, context_t *ctx) {
    int ret = 0;

    // determine the type of load/store
    char *f2 = fields->fields[2];
    char *f3 = fields->fields[3];
    if (f2[0] != '[') {
        inst->type = INST_LDLT;
    } else if (fields->count == 3) {
        inst->type = INST_SDTU;
    } else if (strchr(f2, ']') || strchr(f3, '!')) {
        inst->type = INST_SDTI;
    } else if (f3[0] == '#') {
        inst->type = INST_SDTU;
    } else {
        inst->type = INST_SDTR;
    }

    // build intermediate instruction
    char *f_rt = fields->fields[1];
    char *f_xn = f2;
    char *f_literal = f2;
    char *f_imm = f3;
    char *f_xm = f3;
    if (inst->type == INST_LDLT) {
        if (ret == 0) {
            ret = parse_reg(&inst->ldlt.rt, f_rt, &inst->ldlt.sf);
        }
        if (ret == 0) {
            ret = parse_addr(&inst->ldlt.literal, f_literal, ctx);
        }
    } else if (inst->type == INST_SDTI) {
        if (strchr(f_imm, '!')) {
            *strchr(f_imm, ']') = '\0';
            inst->sdti.pre = true;
        } else {
            *strchr(f_xn, ']') = '\0';
            inst->sdti.pre = false;
        }

        if (ret == 0) {
            ret = parse_reg(&inst->sdti.rt, f_rt, &inst->sdti.sf);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->sdti.xn, f_xn + 1, NULL);
        }
        if (ret == 0) {
            ret = parse_simm(&inst->sdti.simm, f_imm);
        }
    } else if (inst->type == INST_SDTU) {
        if (ret == 0) {
            ret = parse_reg(&inst->sdtu.rt, f_rt, &inst->sdtu.sf);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->sdtu.xn, f_xn + 1, NULL);
        }
        if (ret == 0 && f_imm) {
            *strchr(f_imm, ']') = '\0';
            ret = parse_imm(&inst->sdtu.imm, f_imm);
        }
    } else {
        *strchr(f_xm, ']') = '\0';
        if (ret == 0) {
            ret = parse_reg(&inst->sdtr.rt, f_rt, &inst->sdtr.sf);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->sdtr.xn, f_xn + 1, NULL);
        }
        if (ret == 0) {
            ret = parse_reg(&inst->sdtr.xm, f_xm, NULL);
        }
    }

    return ret;
}

static int parse_nop(fields_t *fields, inst_t *inst, context_t *ctx) {
    inst->type = INST_SPEC;
    (void)fields;
    (void)inst;
    (void)ctx;
    return 0;
}

static int parse_int(fields_t *fields, inst_t *inst, context_t *ctx) {
    inst->type = INST_SPEC;
    (void)ctx;
    char *f_data = fields->fields[1];

    long num;
    int ret = parse_number(&num, f_data);
    if (ret == 0) {
        inst->data.data = (int32_t)num;
    }
    return ret;
}
