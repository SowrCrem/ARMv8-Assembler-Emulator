#ifndef ARMV8_47_SYMBOL_TABLE_H
#define ARMV8_47_SYMBOL_TABLE_H

#include "types.h"

int split_line(char line[LINE_MAX], fields_t *fields);
void add_symbol(context_t *ctx, const char *name);
extern uint64_t lookup_symbol(const context_t *ctx, const char *name);
void init_context(context_t *ctx);
void free_context(context_t *ctx);

#endif //ARMV8_47_SYMBOL_TABLE_H
