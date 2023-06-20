// Tokenizer recieves instructions and breaks them into a list of tokens

#include "tokenizer.h"

token_list_t *create_token_list(void)
{
    token_list_t *tokens = (token_list_t *)malloc(sizeof(token_list_t));
    if (!tokens)
    {
        perror("Unable to allocate memory to token list.\n");
        exit(EXIT_FAILURE);
    }
    tokens->head = NULL;

    tokens->size = 0;
    tokens->no_labels = 0;

    return tokens;
}

void add_token_to_token_list(token_list_t *tokens, token_t *token)
{
    if (tokens->head == NULL)
    {
        // The linked-list is empty
        tokens->head = token;
        tokens->size++;
        return;
    }

    token_t *curr = tokens->head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }
    curr->next = token;
    tokens->size++;
}

void destroy_token_list(token_list_t *tokens)
{
    token_t *curr = tokens->head;
    while (curr != NULL)
    {
        token_t *tmp = curr;
        curr = curr->next;

        if (tmp->is_label)
        {
            free(tmp->label);
        }
        else
        {
            destroy_2d_array(tmp->operands, tmp->no_operands);
            free(tmp->opcode);
        }
        free(tmp);
    }

    free(tokens);
}

static bool is_label(char *instruction)
{
    return *(instruction + strlen(instruction) - 1) == ':';
}

static void *tokenize_label(token_t *token, char *label)
{
    token->is_label = true;

    strncpy(token->label, label, MAX_LINE_LENGTH);
    token->label[strlen(label) - 1] = '\0';

    return token;
}

static void *tokenize_instruction(token_t *token, char *instruction)
{
    token->is_label = false;

    char *opcode = strtok_r(instruction, " ", &instruction);
    token->opcode = strcpy(token->opcode, opcode);

    int i = 0;
    char *curr_token = initialise_array(MAX_LINE_LENGTH);
    do
    {

        // Strip off leading whitespace/separators on token
        while (*instruction == ' ' || *instruction == ',' || *instruction == ']')
        {
            instruction++;
        }

        char *curr_substr;
        if (*instruction == '[')
        {
            curr_substr = strtok_r(instruction, "]", &instruction);

            if (curr_substr == NULL)
            {
                break;
            }

            strcpy(curr_token, curr_substr);
            char close_bracket = ']';
            strncat(curr_token, &close_bracket, 1);
        }
        else
        {
            curr_substr = strtok_r(instruction, " ,", &instruction);

            if (curr_substr == NULL)
            {
                break;
            }

            strcpy(curr_token, curr_substr);
        }
        strcpy(token->operands[i], curr_token);
        i++;
    } while (curr_token != NULL && *instruction != '\0');
    free(curr_token);
    token->no_operands = i;

    return token;
}

// Processes instructions into tokens and adds to list of tokens
void tokenize(token_list_t *tokens, char **instructions, int no_instructions, symbol_table_t *table)
{
    unsigned int current_address = 0;

    for (int i = 0; i < no_instructions; i++)
    {
        if (instructions[i][0] != '\n')
        {
            token_t *token = malloc(sizeof(token_t));
            *token = (token_t){
                    .is_label = false,
                    .label = NULL,
                    .opcode = NULL,
                    .operands = NULL,
                    .no_operands = 0,
                    .next = NULL};

            if (!token)
            {
                perror("Unable to allocate memory to token.\n");
                exit(EXIT_FAILURE);
            }

            token->label = initialise_array(MAX_LINE_LENGTH);
            token->opcode = initialise_array(MAX_LINE_LENGTH);
            token->operands = initialise_2d_array(MAX_NUMBER_OF_OPERANDS, MAX_LINE_LENGTH);

            if (is_label(instructions[i]))
            {
                tokenize_label(token, instructions[i]);
                tokens->no_labels++;
                add_label_to_symbol_table(table, token->label, current_address);
            }
            else
            {
                tokenize_instruction(token, instructions[i]);
                current_address += INSTRUCTION_ADDRESS_SIZE;
            }

            add_token_to_token_list(tokens, token);
        }
    }
}
