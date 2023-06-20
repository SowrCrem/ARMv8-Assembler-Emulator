// Functions for converting a token to an instruction

#include "token_to_instruction.h"
#define NO_MNEMONICS 23

// TODO: confirm the operands are converted as expected

const mnemonic_instruction_t mnemonic_table[] =
        {{.mnemonic = "add", .type = DATA_PROCESSING, .opcode = ADD},
         {.mnemonic = "sub", .type = DATA_PROCESSING, .opcode = SUB},
         {.mnemonic = "rsb", .type = DATA_PROCESSING, .opcode = RSB},
         {.mnemonic = "and", .type = DATA_PROCESSING, .opcode = AND},
         {.mnemonic = "eor", .type = DATA_PROCESSING, .opcode = EOR},
         {.mnemonic = "orr", .type = DATA_PROCESSING, .opcode = ORR},
         {.mnemonic = "mov", .type = DATA_PROCESSING, .opcode = MOV},
         {.mnemonic = "tst", .type = DATA_PROCESSING, .opcode = TST},
         {.mnemonic = "teq", .type = DATA_PROCESSING, .opcode = TEQ},
         {.mnemonic = "cmp", .type = DATA_PROCESSING, .opcode = CMP},
         {.mnemonic = "mul", .type = MULTIPLY},
         {.mnemonic = "mla", .type = MULTIPLY},
         {.mnemonic = "ldr", .type = SINGLE_DATA_TRANSFER},
         {.mnemonic = "str", .type = SINGLE_DATA_TRANSFER},
         {.mnemonic = "beq", .type = BRANCH_2, .cond = EQ},
         {.mnemonic = "bne", .type = BRANCH_2, .cond = NE},
         {.mnemonic = "bge", .type = BRANCH_2, .cond = GE},
         {.mnemonic = "blt", .type = BRANCH_2, .cond = LT},
         {.mnemonic = "bgt", .type = BRANCH_2, .cond = GT},
         {.mnemonic = "ble", .type = BRANCH_2, .cond = LE},
         {.mnemonic = "b", .type = BRANCH_2, .cond = AL},
         {.mnemonic = "lsl", .type = SPECIAL},
         {.mnemonic = "andeq", .type = SPECIAL}};

static mnemonic_instruction_t get_instruction(char *mnemonic)
{
    for (int i = 0; i < NO_MNEMONICS; i++)
    {
        if (!strcmp(mnemonic, mnemonic_table[i].mnemonic))
        {
            return mnemonic_table[i];
        }
    }

    if (DEBUG)
    {
        printf("Invalid instruction mnemonic: %s.\n", mnemonic);
    }
    exit(EXIT_FAILURE);
}

static word_t operand_to_int(char *operand)
{
    return atoi(operand + 1);
}

// function to convert hex/dec string into uint_32
static word_t evaluate_expression(char *expression, instruction_t *instruction)
{
    expression++;
    if (expression[0] == '-')
    {
        instruction->flag_u = false;
        if (expression[2] == 'x')
        {
            return strtol(expression + 1, NULL, 16);
        }
        else
        {
            return strtol(expression + 1, NULL, 10); // expression is in decimal
        }
    }
    if (expression[1] == 'x')
    {
        return strtol(expression, NULL, 16); // expression is in hexadecimal
    }
    else
    {
        return strtol(expression, NULL, 10); // expression is in decimal
    }
}

static void decode_operand_two_immediate(token_t *token, instruction_t *instruction, int operand_two_location)
{
    // if less than 2^8 bits then we can just set with no shift amout
    // if can be made by rotating then we will set rotate bits and rotate
    // otherwise error
    instruction->shift_type = ROR;
    word_t imm = evaluate_expression(token->operands[operand_two_location], instruction);
    if (imm <= 0xFF)
    {
        instruction->shift_amount = 0;
        if (instruction->type == DATA_PROCESSING)
        {
            instruction->imm = imm;
        }
        else
        {
            instruction->offset = imm;
        }
        return;
    }
    bool found = false;
    // for (int i = 1; i < WORD_SIZE; i++)
    unsigned int i = WORD_SIZE;
    ;
    while (!(imm & MASK_2_LSB))
    {
        i--;
        imm >>= 2;

        /*
        word_t shifted_bits = imm >> i;
        word_t rotated_bits = imm << (WORD_SIZE - i);
        word_t rotated = shifted_bits | rotated_bits;
        if (rotated <= 0xFF && instruction->type == DATA_PROCESSING)
        {
            instruction->shift_amount = (WORD_SIZE - i) >> 1;
            instruction->imm = rotated;
            found = true;
            break;
        }
        if (rotated <= 0xFF && instruction->type == SINGLE_DATA_TRANSFER)
        {
            instruction->shift_amount = (WORD_SIZE - i) >> 1;
            instruction->offset = rotated;
            found = true;
            break;
        }
        */
    }

    if (imm <= 0xFF && instruction->type == DATA_PROCESSING)
    {
        instruction->shift_amount = i;
        instruction->imm = imm;
        found = true;
    }
    if (imm <= 0xFF && instruction->type == SINGLE_DATA_TRANSFER)
    {
        instruction->shift_amount = i;
        instruction->offset = imm;
        found = true;
    }

    if (!found)
    {
        perror("given immediate cannot be expressed as a 8 bit number rotated");
    }
}

static void decode_operand_two(token_t *token, instruction_t *instruction, int operand_two_location)
{
    if (token->operands[operand_two_location][0] == '#')
    {
        if (instruction->type == DATA_PROCESSING)
        {
            instruction->flag_i = true;
        }
        // TODO: Implement offset logic
        decode_operand_two_immediate(token, instruction, operand_two_location);
    }
    else
    {
        // Is a shift
        instruction->rm = operand_to_int(token->operands[operand_two_location]);
        if (instruction->type == SINGLE_DATA_TRANSFER)
        {
            instruction->flag_i = true;
        }
        if (token->no_operands - 1 == operand_two_location)
        {
            return;
        }

        if (!strcmp("lsl", token->operands[operand_two_location + 1]))
        {
            instruction->shift_type = LSL;
        }
        else if (!strcmp("asr", token->operands[operand_two_location + 1]))
        {
            instruction->shift_type = ASR;
        }
        else if (!strcmp("lsr", token->operands[operand_two_location + 1]))
        {
            instruction->shift_type = LSR;
        }
        else if (!strcmp("ror", token->operands[operand_two_location + 1]))
        {
            instruction->shift_type = ROR;
        }
        else
        {
            perror("unrecognized shift type");
        }
        if (token->operands[operand_two_location + 2][0] == 'r')
        {
            instruction->rs = operand_to_int(token->operands[operand_two_location + 2]);
            instruction->use_rs = true;
        }
        else
        {
            instruction->shift_amount = evaluate_expression(token->operands[operand_two_location + 2], instruction);
        }
    }
}

static void convert_dpi_to_instruction(token_t *token, instruction_t *instruction)
{
    instruction->cond = AL;
    switch (instruction->opcode)
    {
        case EOR:
        case SUB:
        case RSB:
        case ADD:
        case ORR:
        case AND:
            instruction->flag_s = false;
            instruction->rd = operand_to_int(token->operands[0]);
            instruction->rn = operand_to_int(token->operands[1]);
            // decode_operand_two should also set I flag
            decode_operand_two(token, instruction, 2);
            break;
        case MOV:
            instruction->flag_s = false;
            instruction->rd = operand_to_int(token->operands[0]);
            decode_operand_two(token, instruction, 1);
            break;
        case TST:
        case TEQ:
        case CMP:
            instruction->flag_s = true;
            instruction->rn = operand_to_int(token->operands[0]);
            decode_operand_two(token, instruction, 1);
            break;
        default:
            perror("unknown opcode\n");
            break;
    }
}

static void convert_multiply_instruction(token_t *token, instruction_t *instruction)
{
    instruction->rn = 0;
    instruction->rd = operand_to_int(token->operands[0]);
    instruction->rm = operand_to_int(token->operands[1]);
    instruction->rs = operand_to_int(token->operands[2]);
    instruction->use_rs = true;
    instruction->flag_s = false;
    instruction->cond = AL;

    if (!strcmp(token->opcode, "mla"))
    {
        instruction->rn = operand_to_int(token->operands[3]);
        instruction->flag_a = true;
    }
}

static void add_to_end_words(word_t *end_words, word_t imm)
{
    int i = 0;
    for (; end_words[i]; i++)
        continue;
    end_words[i] = imm;
}

static void decode_offset_sdt_pre(token_t *token, instruction_t *instruction)
{
    // Pre-indexed

    // Remove brackets:
    int len = strlen(token->operands[1]);
    // token->operands[1][len - 1] = '\0'; // gone to get tea, basically we cannot change individual characters
    // token->operands[1]++;

    char *operand_two = initialise_array(len);
    // char *operand_two_start = &operand_two[0];

    strcpy(operand_two, token->operands[1]);
    operand_two++;
    operand_two[len - 2] = '\0';

    int no_operands = 0;
    // change 4 to non magic (need to merge)
    char **operands = initialise_2d_array(4, MAX_LINE_LENGTH);
    // bool is_bracket = false;
    do
    {
        // Strip off leading whitespace/separators on token
        while (*operand_two == ' ' || *operand_two == ',')
        {
            operand_two++;
        }

        strcpy(operands[no_operands], strtok_r(operand_two, " ,", &operand_two));

        no_operands++;

    } while (operands[no_operands] != NULL && *operand_two != '\0');

    instruction->rn = operand_to_int(operands[0]);
    if (no_operands == 2)
    {
        if (operands[1][0] == 'r')
        {
            instruction->flag_i = true;
            // add with no shift
            instruction->shift_type = 0;
            instruction->shift_amount = 0;
            instruction->rm = operand_to_int(operands[1]);
        }
        else
        {
            token->operands[2] = operands[1];
            token->no_operands = 3;
            decode_operand_two_immediate(token, instruction, 2);
        }
    }
    if (no_operands == 4)
    {
        instruction->flag_i = true;
        if (operands[1][0] == '-' || operands[1][0] == '+')
        {
            if (operands[1][0] == '-')
            {
                instruction->flag_u = false;
            }
            operands[1]++;
            instruction->rm = operand_to_int(operands[1]);
        }

        token->operands[2] = operands[1];
        token->operands[3] = operands[2];
        token->operands[4] = operands[3];
        token->no_operands = 5;

        decode_operand_two(token, instruction, 2);
    }

    // free(operand_two_start);
    // destroy_2d_array(operands, 4);
}

static void convert_sdt_instruction(token_t *token, instruction_t *instruction, word_t *end_words, int *no_end_words, int line_no, int stripped_size)
{
    instruction->rd = operand_to_int(token->operands[0]);

    instruction->cond = AL;

    instruction->flag_u = true;
    if (!strcmp(token->opcode, "ldr"))
    {
        instruction->flag_l = true;
    }
    if (token->no_operands > 2)
    {
        // indicates it is post-indexed
        // [Rn],<#expression>
        // [Rn],{+/-}Rm{,<shift>}

        // Remove brackets from register:
        int len_op_two = strlen(token->operands[1]);
        char *operand_two = initialise_array(len_op_two);
        strcpy(operand_two, token->operands[1]);
        operand_two[len_op_two - 1] = '\0';
        operand_two++;
        instruction->rn = operand_to_int(operand_two);
        // remove sign
        int len_op_three = strlen(token->operands[2]);
        char *operand_three = initialise_array(len_op_three);
        strcpy(operand_three, token->operands[2]);
        operand_three[len_op_three - 2] = '\0';
        operand_three++;
        if (operand_three[0] == '-' || operand_three[0] == '+')
        {
            if (token->operands[2][0] == '-')
            {
                instruction->flag_u = false;
            }
            operand_three++;
            token->operands[2] = operand_three;
            instruction->rm = operand_to_int(token->operands[2]);
        }

        // Implement Up bit logic inside function
        decode_operand_two(token, instruction, 2);
    }
    else
    {
        // Pre-indexed
        instruction->flag_p = true;

        // TODO: case where word at end of file
        // implement using end_words

        // <=expression> -> if expression_val <= 0xFF then make into "mov" token and call dpi function
        if (token->operands[1][0] == '=')
        {
            instruction->flag_i = false;
            word_t imm = evaluate_expression(token->operands[1], instruction);
            for (int i = 0; i < WORD_SIZE; i++)
            {
                word_t shifted_bits = imm >> i;
                word_t rotated_bits = imm << (WORD_SIZE - i);
                word_t rotated = shifted_bits | rotated_bits;
                if (rotated <= 0xFF)
                {
                    // reassign to mov rd shifted_imm
                    instruction->opcode = MOV;
                    instruction->shift_amount = WORD_SIZE - i;
                    instruction->imm = rotated;
                    instruction->flag_i = true;
                    instruction->type = DATA_PROCESSING;
                    return;
                }
            }
            instruction->rn = readPC();
            // instruction->offset = (stripped_size - (line_no + 2)) * 4;
            instruction->offset = (stripped_size + *no_end_words) * 4 - (line_no * 4) - 8;
            (*no_end_words)++;
            add_to_end_words(end_words, imm);
            return;
        }
        // [Rn]
        // [Rn,<#expression>]
        // [Rn,{+/-}Rm{,<shift>}]
        decode_offset_sdt_pre(token, instruction);
    }
}

static void convert_branch_instruction(token_t *token, instruction_t *instruction, int line_no, int stripped_size, symbol_table_t *symbol_table)
{
    // expression is target address which may be label
    // compute offset between current ddress and label
    // - 8 and shift 2 right result
    // CHECK IF LABEL
    // address is bytewise

    word_t branch_address;
    if (token->operands[0][0] == '#')
    {
        branch_address = evaluate_expression(token->operands[0], instruction);
    }
    else
    {
        branch_address = (word_t)get_address_in_symbol_table(symbol_table, token->operands[0]);
    }

    word_t curr_address = ((word_t)(line_no << 2)) + INSTRUCTION_ADDRESS_SIZE * 2;

    // check if address is in front of or behind
    word_t offset;
    if (branch_address >= curr_address)
    {
        offset = branch_address - curr_address;
    }
    else
    {
        offset = ~(curr_address - branch_address) + 1;
    }
    instruction->offset = offset >> 2;
}

static void convert_special_to_instruction(token_t *token, instruction_t *instruction)
{
    if (!strcmp(token->opcode, "andeq"))
    {
        instruction->type = HALT;
    }
    else if (!strcmp(token->opcode, "lsl"))
    {
        instruction->type = DATA_PROCESSING;
        instruction->opcode = MOV;
        strcpy(token->opcode, "mov");
        strcpy(token->operands[3], token->operands[1]);
        strcpy(token->operands[2], "lsl");
        strcpy(token->operands[1], token->operands[0]);
        token->no_operands = 4;

        // instruction->rd = operand_to_int(token->operands[0]);
        // decode_operand_two(token, instruction, 1);
        convert_dpi_to_instruction(token, instruction);
    }
    else
    {
        perror("unrecognized type");
        exit(EXIT_FAILURE);
    }
}

// size should be size - no_lines
instruction_t *token_to_instruction(token_t *token, word_t *end_words, int *no_end_words, int line_no, int stripped_size, symbol_table_t *symbol_table)
{
    // remember to free
    // assumes not label
    instruction_t *instruction = malloc(sizeof(instruction_t));
    if (!instruction)
    {
        perror("Unable to allocate memory to instruction.\n");
        exit(EXIT_FAILURE);
    }

    *instruction = (instruction_t){
            .type = NOT_PRESENT,
            .rn = -1,
            .rd = -1,
            .rs = -1,
            .rm = -1,
            .flag_i = false,
            .flag_a = false,
            .flag_s = false,
            .flag_l = false,
            .flag_p = false,
            .flag_u = false,
            .use_rs = false,
            .shift_type = 0,
            .shift_amount = 0,
            .imm = 0,
            .offset = 0,
    };

    mnemonic_instruction_t mnemonic = get_instruction(token->opcode);
    instruction->type = mnemonic.type;
    switch (mnemonic.type)
    {
        case DATA_PROCESSING:
            instruction->opcode = mnemonic.opcode;
            convert_dpi_to_instruction(token, instruction);
            break;
        case MULTIPLY:
            convert_multiply_instruction(token, instruction);
            break;
        case SINGLE_DATA_TRANSFER:
            convert_sdt_instruction(token, instruction, end_words, no_end_words, line_no, stripped_size);
            break;
        case BRANCH_2:
            instruction->cond = mnemonic.cond;
            convert_branch_instruction(token, instruction, line_no, stripped_size, symbol_table);
            break;
        case SPECIAL:
            convert_special_to_instruction(token, instruction);
            break;
        default:
            perror("unrecognized type\n");
    }
    return instruction;
}