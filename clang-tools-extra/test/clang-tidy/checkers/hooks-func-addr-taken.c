// RUN: %check_clang_tidy %s hooks-func-addr-taken %t

#include <stdint.h>

extern int64_t etxn_burden();

extern int64_t ledger_seq();

int64_t hook(int64_t reserved)
{
    return (reserved ? etxn_burden : ledger_seq)();
// CHECK-MESSAGES: :[[@LINE-1]]:13: warning: indirect function calls not supported [hooks-func-addr-taken]
}
