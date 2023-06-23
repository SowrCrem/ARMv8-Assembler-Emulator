#include <string.h>
#include "symbol_table.h"


int split_line(char line[LINE_MAX], fields_t *fields) {
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

void add_symbol(context_t *ctx, const char *name) {
    ctx->tbl.count += 1;
    ctx->tbl.names = realloc(ctx->tbl.names, ctx->tbl.count * sizeof(char *));
    ctx->tbl.addrs = realloc(ctx->tbl.addrs, ctx->tbl.count * sizeof(uint64_t));
    ctx->tbl.names[ctx->tbl.count - 1] = strdup(name);
    ctx->tbl.addrs[ctx->tbl.count - 1] = ctx->pc;
}

uint64_t lookup_symbol(const context_t *ctx, const char *name) {
    for (int i = 0; i < ctx->tbl.count; i++) {
        if (strcmp(ctx->tbl.names[i], name) == 0) {
            return ctx->tbl.addrs[i];
        }
    }

    return (uint64_t)-1;
}

void init_context(context_t *ctx) {
    memset(ctx, 0, sizeof(context_t));
}

void free_context(context_t *ctx) {
    for (int i = 0; i < ctx->tbl.count; i++) {
        free(ctx->tbl.names[i]);
    }
    free(ctx->tbl.names);
    free(ctx->tbl.addrs);
}

