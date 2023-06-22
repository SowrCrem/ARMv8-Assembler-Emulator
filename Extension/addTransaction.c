//
// Created by shlok on 22/06/2023.
//

#include "addTransaction.h"
#include "utils.h"
#include <stdio.h>
#include "string.h"


Ledger addTransaction(int amount, char paidby[], char splitBetween[], char reference[], Ledger myledger) {
    char* emptypeople[MAXNOPPL];
    stringArray pplSplitbetween = splitStringByComma(splitBetween, emptypeople);
    char** splitnames = pplSplitbetween.array;
    int nosplitwith = pplSplitbetween.no;
    Transaction t1;
    t1.amount = amount;
    strcpy(t1.paidby, paidby);
    for (int i = 0; i < nosplitwith; ++i) {
        strcpy(t1.split_between[i], splitnames[i]);
    }

    strcpy(t1.reference, reference);
    myledger.transactions[myledger.numTransactions] = t1;
    myledger.numTransactions++;
    return myledger;

}