#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include <stdbool.h>
#include "addTransaction.h"
#include "showTransaction.h"
#include "settleBills.h"


#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))


int main() {
    Ledger myledger;
    myledger.numTransactions = 0;

    char stringPeople[MAX_REFERENCE_LENGTH];
    char* EmptyArrayNames[MAXNOPPL];

    printf("Enter the people in the ledger seperated by just a comma\n");
    scanf("%s", stringPeople);

    stringArray s1 = splitStringByComma(stringPeople, EmptyArrayNames);
    char** names = s1.array;
    int noPeople = s1.no;
    bool end = false;
    while (!end) {

        printf("What do you want to do\n1:Add a transaction\n2:Show all transactions\n3:Settle bills\n4:Quit"  );

        char toDo[1];
        scanf("%s", toDo);

        char Stringamount[MAX_REFERENCE_LENGTH];
        int amount;
        char paidby[MAX_NAME_LENGTH];
        char splitBetween[MAX_REFERENCE_LENGTH];
        char reference[MAX_REFERENCE_LENGTH];
        char perspective[MAX_NAME_LENGTH];

        switch (atoi(toDo)) {
            case 1:
                printf("How much was paid ");
                scanf("%s", Stringamount);
                amount = atoi(Stringamount);
                printf("Who paid it ");
                scanf("%s", paidby);
                printf("Who has it split between (seperate names with commas) ");
                scanf("%s", splitBetween);
                printf("What is the reference ");
                scanf("%s", reference);
                myledger = addTransaction(amount, paidby, splitBetween, reference, myledger);
                break;
            case 2:
                showTransactions(myledger);
            case 3:
                printf("Who do you want to see who owes money ");
                scanf("%s", perspective);
                settleBills(myledger, names, noPeople, perspective);
            case 4:
                end = true;
        }
    }
//    printf("%d",myledger.numTransactions);
//    for (int i = 0; i < myledger.numTransactions; i++) {
//        printf("%d",i);
//        Transaction t1 = myledger.transactions[i];
//        printf("%d %s %s", t1.amount, t1.reference, t1.paidby);
//        for (int j = 0; j < noPeople; ++j) {
//            printf("\n\n%s",t1.split_between[j]);
//        }
//    }

    return 0;
}
