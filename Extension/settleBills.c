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
    for (int i = 0; i < noPeople; i++) {
        nameBalance nb;
        if (perspective != names[i]) {
            nb.balance = 0;
            strcpy(nb.name, names[i]);
        }
    }

    for(int i = 0; i < myLedger.numTransactions; i++) {
        Transaction t = myLedger.transactions[i];
        //When perspective has paid the amount
        if (t.paidby == perspective) {
            int each = t.amount / sizeStringArray(t.split_between);
            for (int j = 0; j < noPeople; ++j) {
                nameBalance curr = finalBalance[i];
                if (isStringInList(curr.name, t.split_between, noPeople)) {
                    curr.balance = curr.balance + each;
                }
            }
        //When perspective only in people split between
        } else if (isStringInList(perspective, t.split_between, noPeople)) {
            int each = t.amount/ sizeStringArray(t.split_between);
            for (int j = 0; j < noPeople; ++j) {
                if(finalBalance[j].name == t.paidby) {
                    finalBalance[j].balance = finalBalance[j].balance - each;
                }
            }
        }
    }

    for (int i = 0; i < noPeople; ++i) {
        char str[MAX_NAME_LENGTH];
        finalBalance[i].balance > 0 ? strcpy(str, "owes") : strcpy(str, "is owed");

        printf("%s %s £%d to %s", perspective, str, finalBalance[i].balance, finalBalance[i].name);

    }



}