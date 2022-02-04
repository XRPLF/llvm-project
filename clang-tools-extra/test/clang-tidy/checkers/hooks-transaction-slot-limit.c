// RUN: %check_clang_tidy %s hooks-transaction-slot-limit %t

#include <stdint.h>

extern int64_t otxn_slot(uint32_t slot);

int64_t hook(int64_t reserved)
{
    otxn_slot(0);

    otxn_slot(-1);
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: function otxn_slot may not use more than 255 slots [hooks-transaction-slot-limit]

    return reserved;
}
