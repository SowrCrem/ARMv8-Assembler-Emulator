#ifndef ARMV8_47_TYPES_H
#define ARMV8_47_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Constants for assembler
#define LINE_MAX 256
#define NAME_MAX 32
#define FIELDS_MAX 7

typedef enum {lalala} op_t;

// Structure check assemble to create
typedef struct {} inst_t;

typedef struct {
    int count;
    fields_t fields;
} fields_t;

typedef struct {
    int count;
    char *names;
    int *addrs;
} table_t;

typedef struct {
    table_t tbl;
    int pc;
} context_t;

#endif //ARMV8_47_TYPES_H
