// Definition of a linked-list type for the symbol table

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "../global_var.h"
#include "../utils/utils.h"
#include <string.h>

typedef struct symbol_table_node
{
    address_t address;
    char *label;

    struct symbol_table_node *next;
} symbol_table_node_t;

typedef struct
{
    symbol_table_node_t *head;
    int size;
} symbol_table_t;

symbol_table_t *create_symbol_table(void);
void add_label_to_symbol_table(symbol_table_t *table, char *label, address_t address);
address_t get_address_in_symbol_table(symbol_table_t *table, char *label);
void destroy_symbol_table(symbol_table_t *table);

#endif
