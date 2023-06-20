// Headerfile for parser.c

#ifndef PARSE_H
#define PARSE_H
#include "../utils/utils.h"
#include "../utils/storage.h"
#include "symbol_table.h"
#include "tokenizer.h"
#include "instruction.h"

typedef struct mnemonic_instruction
{

    char mnemonic[5];

    instruction_type_t type;
    opcode_t opcode;
    cond_t cond;

} mnemonic_instruction_t;
extern const mnemonic_instruction_t mnemonic_table[];

instruction_t *token_to_instruction(token_t *token, word_t *end_words, int *no_end_words, int current_address, int stripped_size, symbol_table_t *symbol_table);

#endif
