// RUN: %check_clang_tidy %s hooks-fee-prereq %t

#include <stdint.h>

extern int64_t etxn_fee_base(uint32_t tx_byte_count);

extern int64_t etxn_reserve(uint32_t count);

// warning not emitted if the prerequisite _could_ be called - that's
// on the programmer (and run-time checks in rippled)...
int64_t hook2(int64_t reserved)
{
    if (reserved)
        etxn_reserve(0);

    return etxn_fee_base(0);
}

int64_t hook(int64_t reserved)
{
    uint8_t emitdet[105];
    return etxn_fee_base(0);
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: call of etxn_fee_base must be preceded by call of etxn_reserve [hooks-fee-prereq]
}
