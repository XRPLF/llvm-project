// RUN: %check_clang_tidy %s hooks-reserve-limit %t

#include <stdint.h>

extern int64_t etxn_reserve(uint32_t count);

int64_t hook(int64_t reserved)
{
    etxn_reserve(1);

    // repeated calls to etxn_reserve aren't correct either, but
    // that's a different check...
    etxn_reserve(1000);
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: etxn_reserve may not reserve more than 255 transactions [hooks-reserve-limit]

    etxn_reserve(0);
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: etxn_reserve need not be called to reserve 0 transactions [hooks-reserve-limit]

    // variable argument cannot be checked at compile time
    etxn_reserve(reserved);
}
