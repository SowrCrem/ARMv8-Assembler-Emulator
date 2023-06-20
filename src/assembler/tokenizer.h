// Struct to represent list of tokens to be processed

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "../global_var.h"
#include "../utils/utils.h"
#include "symbol_table.h"

typedef struct token
{

    char *opcode;

    char **operands;
    uint16_t no_operands;

    bool is_label;
    char *label;

    struct token *next;

} token_t;

typedef struct token_list
{

    token_t *head;

    int size;
    int no_labels;

} token_list_t;

token_list_t *create_token_list(void);
void add_token_to_token_list(token_list_t *tokens, token_t *token);
void destroy_token_list(token_list_t *tokens);
void tokenize(token_list_t *tokens, char **input, int input_size, symbol_table_t *table);

#endif
