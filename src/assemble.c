// ARM11 Assembler

#include "assembler/tokenizer.h"
#include "assembler/token_to_instruction.h"
#include "assembler/encoder.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        // Only accept two arguments (input program file)
        fprintf(stderr, "Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    char *source_file = argv[1];
    char *output_file = argv[2];

    // Reading from the input assembly file
    int no_lines = no_lines_text_file(source_file);
    char **text_file = load_text_file(source_file, no_lines);

    if (DEBUG)
    {
        printf("[Text File]: %d line(s)\n", no_lines);
        for (int i = 0; i < no_lines; i++)
        {
            printf("%d: %s\n", i, text_file[i]);
        }
        printf("\n");
    }

    // PASS 1
    token_list_t *tokens = create_token_list();
    symbol_table_t *symbol_table = create_symbol_table();

    tokenize(tokens, text_file, no_lines, symbol_table);

    if (DEBUG)
    {
        printf("===== PASS 1 =====\n");
        printf("[Token List]: %d token(s), %d label(s)\n", tokens->size, tokens->no_labels);

        token_t *curr_debug_token = tokens->head;
        while (curr_debug_token != NULL)
        {
            if (curr_debug_token->is_label)
            {
                printf("Label: %s\n", curr_debug_token->label);
            }
            else
            {
                printf("Opcode: %s\n", curr_debug_token->opcode);
                for (int i = 0; i < curr_debug_token->no_operands; i++)
                {
                    printf("    Operand %d: %s\n", i, curr_debug_token->operands[i]);
                }
            }
            printf("\n");
            curr_debug_token = curr_debug_token->next;
        }

        printf("[Symbol Table]: %d label(s)\n", symbol_table->size);
        symbol_table_node_t *curr_debug_node = symbol_table->head;
        while (curr_debug_node != NULL)
        {
            printf("Label: %s\n", curr_debug_node->label);
            printf("Address: 0x%x\n", curr_debug_node->address);
            printf("\n");
            curr_debug_node = curr_debug_node->next;
        }
        printf("\n");
    }

    // PASS 2
    int no_instructions = tokens->size - tokens->no_labels;
    if (DEBUG)
    {
        printf("===== PASS 2 =====\n");

        printf("Number of instructions: %d\n", no_instructions);
    }

    instruction_t *instruction = malloc(sizeof(instruction_t));
    word_t *binary = malloc(no_instructions * sizeof(word_t));
    word_t *end_words = malloc(no_instructions * sizeof(word_t));
    int no_end_words = 0;

    unsigned int line_no = 0x0;

    token_t *curr_token = tokens->head;
    while (curr_token != NULL)
    {
        if (!curr_token->is_label)
        {
            instruction = token_to_instruction(curr_token, end_words, &no_end_words, line_no, no_instructions, symbol_table);

            if (DEBUG)
            {
                printf("Registers:\n");
                printf("    Rn: %x\n    Rs: %x\n    Rd: %x\n    Rm: %x\n", instruction->rn, instruction->rs, instruction->rd, instruction->rm);
                printf("Shift type: %d\n", instruction->shift_type);
                printf("Shift amount: %d\n", instruction->shift_amount);
                printf("Offset: 0x%x (%d)\n", instruction->offset, instruction->offset);
                printf("Imm: 0x%x, (%d)\n", instruction->imm, instruction->imm);

                printf("Flags:\n");
                printf("    I: %d\n    A: %d\n    S: %d\n    L: %d\n    P: %d\n    U: %d\n", instruction->flag_i, instruction->flag_a, instruction->flag_s, instruction->flag_l, instruction->flag_p, instruction->flag_u);
                printf("Word at bottom: %x \n", end_words[0]);
                printf("\n");
            }

            binary[line_no] = encode(instruction);
            line_no += 1;
        }
        curr_token = curr_token->next;
    }

    // Writing to output file
    if (DEBUG)
    {
        printf("Binary:\n");
        for (unsigned int i = 0; i < no_instructions; i++)
        {
            printf("%08x: %08x\n", 4 * i, binary[i]);
        }
        printf("\n");
    }

    write_binary_file(binary, output_file, no_instructions);

    if (DEBUG)
    {
        printf("Written instructions to binary file.\n");
        printf("\n");
    }

    // Writing end-words to end of file
    FILE *output_fp = fopen(output_file, "ab");
    if (output_fp == NULL)
    {
        perror("Cannot open output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < no_end_words; i++)
    {
        fwrite(&end_words[i], sizeof(word_t), 1, output_fp); // TODO: FIX - Causes valgrind error on end_words

        if (DEBUG)
        {
            printf("End word %d: %x\n", i, end_words[i]);
        }
    }

    fclose(output_fp);

    // Freeing memory
    free(instruction);
    free(end_words);
    free(binary);
    destroy_2d_array(text_file, no_lines);
    destroy_token_list(tokens);
    destroy_symbol_table(symbol_table);

    return EXIT_SUCCESS;
}
