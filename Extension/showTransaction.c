//
// Created by shlok on 22/06/2023.
//

#include "showTransaction.h"
#include "utils.h"
#include <stdio.h>

void showTransactions(Ledger myledger){
    printf("%-8s %-15s %-20s %-15s\n", "Amount", "Paid By", "Split Between", "Reference");
    for(int i = 0; i< myledger.numTransactions; i++) {
        printf("%-8d %-15s ", myledger.transactions[i].amount, myledger.transactions[i].paidby);

        for (int j = 0; j < MAXNOPPL; j++) {
            printf("%-2s", myledger.transactions[i].split_between[j]);
        }
        printf("%-15s\n\n", myledger.transactions[i].reference);
    }

}
