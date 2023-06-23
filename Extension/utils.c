//
// Created by shlok on 21/06/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXNOPPL 10
#define MAX_NAME_LENGTH 20
#define MAX_TRANSACTIONS 500
#define MAX_REFERENCE_LENGTH 100

typedef struct {
    int amount;
    char paidby[MAX_NAME_LENGTH];
    char split_between[MAXNOPPL][MAX_NAME_LENGTH];
    char reference[MAX_REFERENCE_LENGTH];
} Transaction;

typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    int numTransactions
} Ledger;

typedef struct {
    char** array;
    int no;
} stringArray;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int balance;
} nameBalance;

stringArray splitStringByComma(const char* string, char* array[]) {
    int elementCount = 0;
    char* token;

    // Create a copy of the input string since strtok modifies the original string
    char* stringCopy = strdup(string);

    // Get the first token
    token = strtok(stringCopy, ",");

    // Traverse the remaining tokens
    while (token != NULL && elementCount < MAXNOPPL) {
        // Allocate memory for the element
        array[elementCount] = malloc(MAX_NAME_LENGTH * sizeof(char));

        // Copy the token into the array
        strcpy(array[elementCount], token);

        // Get the next token
        token = strtok(NULL, ",");

        elementCount++;
    }

    free(stringCopy); // Free the memory allocated for the copied string
    stringArray toReturn;
    toReturn.array = array;
    toReturn.no = elementCount;
    return toReturn;
}

int sizeStringArray(const char strings[][MAX_NAME_LENGTH]) {
    int count = 0;

    while (strings[count] != NULL) {
        //printf("%s\n\n", strings[count]);
        count++;
    }
    return count;
}

bool isStringInList(const char* target, const char strings[][MAX_NAME_LENGTH], int numStrings) {
    for(int i = 0; i < numStrings; i++) {
        if (strcmp(target, strings[i]) == 0) {
            return true;
        }
    }
    return false;
}


