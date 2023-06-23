//
// Created by shlok on 22/06/2023.
//

#include "settleBills.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void settleBills(Ledger myLedger, char** names, int noPeople, char perspective[MAX_NAME_LENGTH]){
    nameBalance finalBalance[noPeople-1];
//Initalise output to have all other people to a name and balance of 0
    int index = 0;
    for (int i = 0; i < noPeople; i++) {
        nameBalance nb;
        if (strcmp(perspective, names[i]) != 0) {
            nb.balance = 0;
            strcpy(nb.name, names[i]);
            finalBalance[index] = nb;
            index++;
        }
    }

    for(int i = 0; i < myLedger.numTransactions; i++) {
        Transaction t = myLedger.transactions[i];
        //When perspective has paid the amount
        printf("%s, %s",t.paidby, perspective);
        if (strcmp(t.paidby, perspective) == 0) {
            int each = t.amount / t.noSplitWith;
            for (int j = 0; j < noPeople; ++j) {
                printf("%d", j);
                nameBalance curr = finalBalance[j];
                if (isStringInList(curr.name, t.split_between, noPeople)) {
                    printf("Getting inside");
                    finalBalance[j].balance += each;
//                    curr.balance = curr.balance + each;
                }
            }
            for (int j = 0; j < noPeople-1; ++j) {
                printf("%s debt %d",finalBalance[j].name, finalBalance[j].balance);
            }

            printf("leaving paidfor part");
        //When perspective only in people split between

        } else if (isStringInList(perspective, t.split_between, noPeople)) {
            int each = t.amount/ t.noSplitWith;
            for (int j = 0; j < noPeople; ++j) {
                if(strcmp(finalBalance[j].name, t.paidby) == 0) {
                    finalBalance[j].balance = finalBalance[j].balance - each;
                }
            }
            printf("Leaving splitbetween part");
        } else {
            ;
        }
    }
    printf("Got to here");

    for (int i = 0; i < noPeople-1; ++i) {
        printf("\n\n%s   %d", finalBalance[i].name, finalBalance[i].balance);
        char owe[MAX_NAME_LENGTH];
        char by[MAX_NAME_LENGTH];
        finalBalance[i].balance >= 0 ? strcpy(owe, "owes") : strcpy(owe, "is owed");
        finalBalance[i].balance >= 0 ? strcpy(by, "by") : strcpy(by, "to");

       printf("%s %s £%d to %s\n\n", perspective, owe, finalBalance[i].balance, finalBalance[i].name);

    }



}