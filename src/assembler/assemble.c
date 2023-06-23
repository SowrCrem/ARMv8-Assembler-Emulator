#define _GNU_SOURCE
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

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

//
// functions for context and symbol tables
//
