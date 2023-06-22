//
// Created by shlok on 22/06/2023.
//


#ifndef EXT_ADDTRANSACTION_H
#define EXT_ADDTRANSACTION_H

#include "utils.h"

Ledger addTransaction(int amount, char paidby[], char splitBetween[], char reference[], Ledger myledger);

#endif //EXT_ADDTRANSACTION_H
