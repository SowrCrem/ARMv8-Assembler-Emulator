// Symbol table implementation

#include "symbol_table.h"

symbol_table_t *create_symbol_table(void)
{
    symbol_table_t *table = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    if (!table)
    {
        perror("Unable to allocate memory to symbol table: exiting.");
        exit(EXIT_FAILURE);
    }
    table->head = NULL;

    table->size = 0;

    return table;
}

void add_label_to_symbol_table(symbol_table_t *table, char *label, address_t address)
{
    symbol_table_node_t *new_entry = malloc(sizeof(symbol_table_node_t));
    if (!new_entry)
    {
        perror("Unable to allocate memory to symbol table node: exiting.");
        exit(EXIT_FAILURE);
    }

    new_entry->address = address;
    new_entry->next = NULL;

    new_entry->label = initialise_array(MAX_LINE_LENGTH);
    strncpy(new_entry->label, label, MAX_LINE_LENGTH);

    if (table->head == NULL)
    {
        table->head = new_entry;
        table->size++;
        return;
    }

    symbol_table_node_t *curr = table->head;
    while (curr->next != NULL)
    {
        if (strcmp(curr->label, label) == 0)
        {
            fprintf(stderr, "Label is already in symbol table.\n");
            exit(EXIT_FAILURE);
        }

        curr = curr->next;
    }
    curr->next = new_entry;
    table->size++;
}

address_t get_address_in_symbol_table(symbol_table_t *table, char *label)
{
    symbol_table_node_t *curr = table->head;

    while (curr != NULL)
    {
        if (strcmp(curr->label, label) == 0)
        {
            return curr->address;
        }

        curr = curr->next;
    }

    // Reached end of linked-list
    perror("Could not find label in table.");
    exit(EXIT_FAILURE);
}

void destroy_symbol_table(symbol_table_t *table)
{
    symbol_table_node_t *curr = table->head;

    symbol_table_node_t *tmp;
    while (curr != NULL)
    {
        tmp = curr;
        curr = curr->next;

        free(tmp->label);
        free(tmp);
    }

    free(table);
}