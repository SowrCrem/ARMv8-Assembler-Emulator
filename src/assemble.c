#define _GNU_SOURCE
#include "cmn.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants for assembler
#define LINE_MAX 256
#define NAME_MAX 32
#define FIELDS_MAX 7

//
// structures for assemble only
//
// struct for symbol tables
typedef struct {
    int count;
    char **names;
    uint64_t *addrs;
} sym_table_t;

// strcut for assemble's context
typedef struct {
    sym_table_t tbl;
    uint64_t pc;
} context_t;

// struct for fields after a line split
typedef struct {
    int count;
    char *fields[FIELDS_MAX];
} fields_t;

// struct for entries of mnemonic parsers
typedef struct {
    char *mnemonic;
    op_t op;
    int (*parse)(fields_t *fields, inst_t *inst, context_t *ctx);
} parser_t;

// main functions
static int assemble(FILE *fin, FILE *fout);
static int pass1(FILE *fin, context_t *ctx);
static int pass2(FILE *fin, FILE *fout, context_t *ctx);
static uint32_t encode(inst_t *inst);

// functions of assemble context & symbol tables
static void add_symbol(context_t *ctx, const char *name);
static uint64_t lookup_symbol(const context_t *ctx, const char *name);
static void init_context(context_t *ctx);
static void free_context(context_t *ctx);

// mnemonics' parsers
static int parse_add_sub(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_cmp(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_neg(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_and_bic_or(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_tst(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_movknz(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_mov(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_mvn(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_madd_msub(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_mul_mneg(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_b(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_br(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_bcond(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_sdr_ldr(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_nop(fields_t *fields, inst_t *inst, context_t *ctx);
static int parse_int(fields_t *fields, inst_t *inst, context_t *ctx);

// encode functions
static uint32_t encode_arii(inst_t *inst);
static uint32_t encode_arir(inst_t *inst);
static uint32_t encode_wdmv(inst_t *inst);
static uint32_t encode_logi(inst_t *inst);
static uint32_t encode_mult(inst_t *inst);
static uint32_t encode_bunc(inst_t *inst);
static uint32_t encode_bcnd(inst_t *inst);
static uint32_t encode_breg(inst_t *inst);
static uint32_t encode_sdti(inst_t *inst);
static uint32_t encode_sdtu(inst_t *inst);
static uint32_t encode_sdtr(inst_t *inst);
static uint32_t encode_ldlt(inst_t *inst);
static uint32_t encode_spec(inst_t *inst);

// helper functions
static int split_line(char line[LINE_MAX], fields_t *fields);

// map of mneomonics and their parsers
const static parser_t parsers[] = {
    {"add", OP_ADD, parse_add_sub},
    {"adds", OP_ADDS, parse_add_sub},
    {"sub", OP_SUB, parse_add_sub},
    {"subs", OP_SUBS, parse_add_sub},
    {"cmp", OP_SUBS, parse_cmp},
    {"cmn", OP_ADDS, parse_cmp},
    {"neg", OP_SUB, parse_neg},
    {"negs", OP_SUBS, parse_neg},
    {"and", OP_AND, parse_and_bic_or},
    {"ands", OP_ANDS, parse_and_bic_or},
    {"bic", OP_BIC, parse_and_bic_or},
    {"bics", OP_BICS, parse_and_bic_or},
    {"eor", OP_EOR, parse_and_bic_or},
    {"orr", OP_ORR, parse_and_bic_or},
    {"eon", OP_EON, parse_and_bic_or},
    {"orn", OP_ORN, parse_and_bic_or},
    {"tst", OP_ANDS, parse_tst},
    {"movk", OP_MOVK, parse_movknz},
    {"movn", OP_MOVN, parse_movknz},
    {"movz", OP_MOVZ, parse_movknz},
    {"mov", OP_ORR, parse_mov},
    {"mvn", OP_ORN, parse_mvn},
    {"madd", OP_MADD, parse_madd_msub},
    {"msub", OP_MSUB, parse_madd_msub},
    {"mul", OP_MADD, parse_mul_mneg},
    {"mneg", OP_MSUB, parse_mul_mneg},
    {"b", OP_B, parse_b},
    {"b.ne", OP_BCOND, parse_bcond},
    {"b.eq", OP_BCOND, parse_bcond},
    {"b.lt", OP_BCOND, parse_bcond},
    {"b.le", OP_BCOND, parse_bcond},
    {"b.gt", OP_BCOND, parse_bcond},
    {"b.ge", OP_BCOND, parse_bcond},
    {"b.al", OP_BCOND, parse_bcond},
    {"br", OP_BR, parse_br},
    {"str", OP_STR, parse_sdr_ldr},
    {"ldr", OP_LDR, parse_sdr_ldr},
    {"nop", OP_NOP, parse_nop},
    {".int", OP_INT, parse_int},
};

//
// main function
//
int main(int argc, char *argv[]) {
    // process in/out
    if (argc != 3) {
        fprintf(stderr, "usage: ./assemble <file_in> <file_out>\n");
        exit(EXIT_FAILURE);
    }

    FILE *fin = fopen(argv[1], "r");
    if (fin == NULL) {
        fprintf(stderr, "Cannot open %s for reading\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    FILE *fout;
    fout = fopen(argv[2], "wb");
    if (fout == NULL) {
        fclose(fin);
        fprintf(stderr, "Cannot open %s for reading\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    // assemble
    assemble(fin, fout);

    // close files
    fclose(fin);
    fclose(fout);

    return 0;
}

// assemble 
static int assemble(FILE *fin, FILE *fout) {
    int ret;

    context_t ctx;
    init_context(&ctx);

    // pass 1 to collect symbols
    ret = pass1(fin, &ctx);
    if (ret == 0) {
        // pass 2 to assemble
        ret = pass2(fin, fout, &ctx);
    }

    free_context(&ctx);
    return ret;
}

// pass 1 to collect symbols and their addresses
static int pass1(FILE *fin, context_t *ctx) {
    fseek(fin, 0, SEEK_SET);
    char line[LINE_MAX];

    ctx->pc = 0;
    while (fgets(line, sizeof(line), fin)) {
        fields_t fields;
        int n_fields = split_line(line, &fields);
        if (n_fields < 0) {
            return -1;
        }

        if (n_fields == 0) {
            continue;
        }

        char *f1 = fields.fields[0];
        if (f1[strlen(f1) - 1] == ':') {
            f1[strlen(f1) - 1] = '\0';
            add_symbol(ctx, f1);
            continue;
        } else {
            ctx->pc += 4;
        }
    }

    return 0;
}

// pass 2 to read the .s file and translate to binary
static int pass2(FILE *fin, FILE *fout, context_t *ctx) {
    int ret = -1;
    int n_insts = 0;
    inst_t **insts = NULL;
    ctx->pc = 0;

    // read .s file and build list of inst_t
    fseek(fin, 0, SEEK_SET);
    char line[LINE_MAX];
    while (fgets(line, sizeof(line), fin)) {
        fields_t fields;
        int n_fields = split_line(line, &fields);

        // error
        if (n_fields < 0) {
            return -1;
        }

        // empyt line
        if (n_fields == 0) {
            continue;
        }

        // label ?
        char *f1 = fields.fields[0];
        if (f1[strlen(f1) - 1] == ':') {
            continue;
        }

        // instruction
        ret = -1;
        inst_t *inst = malloc(sizeof(inst_t));
        for (size_t i = 0; i < sizeof(parsers) / sizeof(parsers[0]); i++) {
            if (strcmp(f1, parsers[i].mnemonic) == 0) {
                inst->op = parsers[i].op;
                ret = parsers[i].parse(&fields, inst, ctx);
                break;
            }
        }

        // save the instruction
        if (ret == 0) {
            ctx->pc += 4;
            n_insts++;
            insts = realloc(insts, sizeof(inst_t *) * n_insts);
            insts[n_insts - 1] = inst;
        } else {
            free(inst);
            break;
        }
    }

    // encode the instructions
    if (ret == 0) {
        uint32_t code;
        for (int i = 0; i < n_insts; i++) {
            code = encode(insts[i]);
            fwrite(&code, sizeof(code), 1, fout);
        }
    }

    // free memory
    for (int i = 0; i < n_insts; i++) {
        free(insts[i]);
    }
    free(insts);

    return ret;
}

// encode
static uint32_t encode(inst_t *inst) {
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

//
// encode funcitons
//
static uint32_t encode_op(op_t op) {
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

static uint32_t encode_arii(inst_t *inst) {

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

static uint32_t encode_arir(inst_t *inst) {
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

static uint32_t encode_wdmv(inst_t *inst) {
    uint32_t op = encode_op(inst->op);
    uint32_t sf = (inst->wdmv.sf ? 1 : 0);
    uint32_t sh_imm = inst->wdmv.sh.imm / 16;
    uint32_t imm = inst->wdmv.imm;
    uint32_t rd = inst->wdmv.rd;

    uint32_t code =
        WDMV_BITS | (sf << 31) | (op << 29) | (sh_imm << 21) | (imm << 5) | rd;
    return code;
};

static uint32_t encode_logi(inst_t *inst) {
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

static uint32_t encode_mult(inst_t *inst) {
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

static uint32_t encode_bunc(inst_t *inst) {
    uint32_t offset = (uint32_t)(inst->bunc.offset / 4);

    uint32_t code = BUNC_BITS;
    code = set_bits(BUNC_BITS, 25, 0, offset);

    return code;
};

static uint32_t encode_bcnd(inst_t *inst) {
    uint32_t offset = (uint32_t)(inst->bcnd.offset / 4);
    uint32_t cond = inst->bcnd.cond;

    uint32_t code = BCND_BITS | cond;
    code = set_bits(code, 23, 5, offset);

    return code;
};

static uint32_t encode_breg(inst_t *inst) {
    uint32_t xn = (uint32_t)(inst->breg.xn);

    uint32_t code = BREG_BITS | (xn << 5);

    return code;
};

static uint32_t encode_sdti(inst_t *inst) {
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

static uint32_t encode_sdtu(inst_t *inst) {
    uint32_t sf = (inst->sdtu.sf ? 1 : 0);
    uint32_t op = encode_op(inst->op);
    uint32_t imm = inst->sdtu.imm / (sf ? 8 : 4);
    uint32_t xn = inst->sdtu.xn;
    uint32_t rt = inst->sdtu.rt;

    uint32_t code =
        SDTU_BITS | (sf << 30) | (op << 22) | (imm << 10) | (xn << 5) | rt;

    return code;
};

static uint32_t encode_sdtr(inst_t *inst) {
    uint32_t sf = (inst->sdtr.sf ? 1 : 0);
    uint32_t op = encode_op(inst->op);
    uint32_t xm = inst->sdtr.xm;
    uint32_t xn = inst->sdtr.xn;
    uint32_t rt = inst->sdtr.rt;

    uint32_t code =
        SDTR_BITS | (sf << 30) | (op << 22) | (xm << 16) | (xn << 5) | rt;

    return code;
};

static uint32_t encode_ldlt(inst_t *inst) {
    uint32_t sf = (inst->ldlt.sf ? 1 : 0);
    uint32_t literal = (uint32_t)(inst->ldlt.literal / 4);
    uint32_t rt = inst->ldlt.rt;

    uint32_t code = LDLT_BITS | (sf << 30) | rt;
    code = set_bits(code, 23, 5, literal);

    return code;
};

static uint32_t encode_spec(inst_t *inst) {
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

//
// functions for context and symbol tables
//
static int split_line(char line[LINE_MAX], fields_t *fields) {
    memset(fields, 0, sizeof(fields_t));

    char *saveptr;
    char *s = line;
    char *field;
    while ((field = strtok_r(s, " ,\n\t", &saveptr))) {
        if (fields->count == FIELDS_MAX) {
            return -1;
        }
        s = NULL;

        fields->fields[fields->count] = field;
        fields->count++;
    }

    return fields->count;
}

static void add_symbol(context_t *ctx, const char *name) {
    ctx->tbl.count += 1;
    ctx->tbl.names = realloc(ctx->tbl.names, ctx->tbl.count * sizeof(char *));
    ctx->tbl.addrs = realloc(ctx->tbl.addrs, ctx->tbl.count * sizeof(uint64_t));
    ctx->tbl.names[ctx->tbl.count - 1] = strdup(name);
    ctx->tbl.addrs[ctx->tbl.count - 1] = ctx->pc;
}

static uint64_t lookup_symbol(const context_t *ctx, const char *name) {
    for (int i = 0; i < ctx->tbl.count; i++) {
        if (strcmp(ctx->tbl.names[i], name) == 0) {
            return ctx->tbl.addrs[i];
        }
    }

    return (uint64_t)-1;
}

static void init_context(context_t *ctx) {
    memset(ctx, 0, sizeof(context_t));
}

static void free_context(context_t *ctx) {
    for (int i = 0; i < ctx->tbl.count; i++) {
        free(ctx->tbl.names[i]);
    }
    free(ctx->tbl.names);
    free(ctx->tbl.addrs);
}