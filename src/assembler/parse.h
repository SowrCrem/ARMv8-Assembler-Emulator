#ifndef ARMV8_47_PARSE_H
#define ARMV8_47_PARSE_H

#include "types.h"

extern int parse_addr(int32_t *offset, char *f_lit, context_t *ctx);
extern int parse_number(long *num, char *s);;
extern int parse_reg(uint8_t *r, char *reg, bool *sf);
extern int parse_imm(uint32_t *u32, char *imm);
extern int parse_simm(int32_t *i32, char *imm);
extern int parse_imm_u16(uint16_t *u16, char *imm);
extern int parse_imm_u8(uint8_t *u8, char *imm);
extern int parse_shift(shift_t *sh, char *sh_type, char *sh_imm);
extern int parse_add_sub(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_cmp(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_neg(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_and_bic_or(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_tst(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_movknz(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_mov(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_mvn(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_madd_msub(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_mul_mneg(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_b(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_bcond(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_br(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_sdr_ldr(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_nop(fields_t *fields, inst_t *inst, context_t *ctx);
extern int parse_int(fields_t *fields, inst_t *inst, context_t *ctx);

#endif //ARMV8_47_PARSE_H
