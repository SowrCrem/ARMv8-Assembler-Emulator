#ifndef ARMV8_47_TYPES_H
#define ARMV8_47_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Constants for assembler
#define LINE_MAX 256
#define NAME_MAX 32
#define FIELDS_MAX 7

static uint32_t ARII_BITS = 0;
static uint32_t ARIR_BITS = 0;
static uint32_t WDMV_BITS = 0;
static uint32_t LOGI_BITS = 0;
static uint32_t MULT_BITS = 0;
static uint32_t BUNC_BITS = 0;
static uint32_t BCND_BITS = 0;
static uint32_t BREG_BITS = 0;
static uint32_t SDTI_BITS = 0;
static uint32_t SDTU_BITS = 0;
static uint32_t SDTR_BITS = 0;
static uint32_t LDLT_BITS = 0;
static uint32_t NOP__BITS = 0;
static uint32_t HALT_BITS = 0;

typedef enum {
    OP_ADD, OP_ADDS, OP_SUB, OP_SUBS, OP_AND, OP_ANDS,
    OP_BIC, OP_BICS, OP_EOR, OP_ORR, OP_EON, OP_ORN,
    OP_MOVK, OP_MOVN, OP_MOVZ, OP_MADD, OP_MSUB, OP_B, OP_HALT,
    OP_BCOND, OP_BR, OP_STR, OP_LDR, OP_NOPOP_INT, OP_NOP, OP_INT
} op_t;

// Enum for Instruction type
typedef enum {
    INST_ARIR, INST_WDMV, INST_LOGI, INST_MULT, INST_BUNC, INST_BCND, INST_BREG, INST_SDTI,
    INST_SDTU, INST_SDTR, INST_LDLT, INST_SPEC, INST_ARII
} instr_type;

// enum for shifts
typedef enum {
    SH_LSL, SH_LSR, SH_ASR, SH_ROR
} shift_type;

// enum for r_
typedef enum {RZR} r_type;

// enum for condition
typedef enum {
    COND_NE, COND_EQ, COND_LT, COND_LE, COND_GE, COND_GT, COND_AL
} cond_t;

typedef struct {
    uint8_t imm;
    uint32_t type;
} shift_t;

typedef struct {
    bool sf;
    shift_t sh;
    int simm;
    uint32_t pre;
    uint8_t xn;
    uint8_t rt;
    uint8_t rm;
} sdti_t;

typedef struct {
    bool sf;
    shift_t sh;
    int simm;
    uint32_t  imm;
    uint32_t pre;
    uint8_t xn;
    uint8_t rt;
    uint8_t rm;
} sdtu_t;

typedef struct {
    bool sf;
    shift_t sh;
    int simm;
    uint16_t imm;
    uint32_t pre;
    uint8_t xn;
    uint8_t xm;
    uint8_t rt;
    uint8_t rm;
} sdtr_t;

typedef struct {
    bool sf;
    shift_t sh;
    int simm;
    uint16_t imm;
    int32_t literal;
    uint32_t pre;
    uint8_t xn;
    uint8_t xm;
    uint8_t rt;
    uint8_t rm;
} ldlt_t;

typedef struct {
    bool sf;
    shift_t sh;
    uint16_t imm;
    uint8_t rn;
    uint8_t rd;
    uint8_t rm;
} wdmv_t;

typedef struct {
    bool sf;
    shift_t sh;
    uint16_t imm;
    uint8_t rn;
    uint8_t rd;
    uint8_t rm;
    uint8_t ra;
} mult_t;

typedef struct {
    bool sf;
    shift_t sh;
    uint16_t imm;
    uint8_t rn;
    uint8_t rd;
    uint8_t rm;
} logi_t;


typedef struct {
    bool sf;
    shift_t sh;
    uint16_t imm;
    uint8_t rn;
    uint8_t rd;
    uint8_t rm;
} arii_t;

typedef struct {
    bool sf;
    shift_t sh;
    uint16_t imm;
    uint8_t rn;
    uint8_t rd;
    uint8_t rm;
} arir_t;

typedef struct {
    int32_t offset;
} bunc_t;

typedef struct {
    int32_t offset;
    uint32_t cond;
} bcnd_t;

typedef struct {
    uint8_t xn;
} breg_t;

typedef struct {
    uint32_t data;
} data_t;

// Structure for instruction
typedef struct {
    op_t op;
    instr_type type;
    arii_t arii;
    arir_t arir;
    shift_t sh;
    wdmv_t wdmv;
    logi_t logi;
    mult_t mult;
    bunc_t bunc;
    bcnd_t bcnd;
    breg_t breg;
    sdti_t sdti;
    sdtu_t sdtu;
    sdtr_t sdtr;
    ldlt_t ldlt;
    data_t data;
} inst_t;

// struct for symbol tables
typedef struct {
    int count;
    char **names;
    uint64_t *addrs;
} sym_table_t;

// struct for assemble context
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

#endif //ARMV8_47_TYPES_H
