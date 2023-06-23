#ifndef ARMV8_47_PARSE_H
#define ARMV8_47_PARSE_H

#include "types.h"

extern int parse_addr(int32_t *offset, char *f_lit, context_t *ctx);
extern int parse_number(long *num, char *s);

#endif //ARMV8_47_PARSE_H
