#ifndef ARMV8_47_ENCODE_H
#define ARMV8_47_ENCODE_H

#include "types.h"

extern uint32_t encode_op(op_t op);
extern uint32_t encode_arii(inst_t *inst);
extern uint32_t encode_arir(inst_t *inst);
extern uint32_t encode_wdmv(inst_t *inst);
extern uint32_t encode_logi(inst_t *inst);
extern uint32_t encode_mult(inst_t *inst);
extern uint32_t set_bits(uint32_t old, int hi, int lo, uint32_t val);
extern uint32_t encode_bunc(inst_t *inst);
extern uint32_t encode_bcnd(inst_t *inst);
extern uint32_t encode_breg(inst_t *inst);
extern uint32_t encode_sdti(inst_t *inst);
extern uint32_t encode_sdtu(inst_t *inst);
extern uint32_t encode_sdtr(inst_t *inst);
extern uint32_t encode_ldlt(inst_t *inst);
extern uint32_t encode_spec(inst_t *inst);
extern uint32_t encode(inst_t *inst);

#endif //ARMV8_47_ENCODE_H
