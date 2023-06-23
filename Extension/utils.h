//
// Created by shlok on 21/06/2023.
//
#ifndef EXT_UTILS_H
#define EXT_UTILS_H
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
    int noSplitWith;
} Transaction;

typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    int numTransactions;
} Ledger;

typedef struct {
    char** array;
    int no;
} stringArray;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int balance;
} nameBalance;


stringArray splitStringByComma(const char* string, char* array[]);

int sizeStringArray(char strings[][MAX_NAME_LENGTH]);

bool isStringInList(const char* target, const char strings[][MAX_NAME_LENGTH], int numString);

#endif