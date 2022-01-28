// RUN: %check_clang_tidy %s hooks-burden-prereq %t

#include <stdint.h>

extern int64_t etxn_burden();

extern int64_t etxn_reserve(uint32_t count);

// warning not emitted if the prerequisite _could_ be called - that's
// on the programmer (and run-time checks in rippled)...
int64_t hook2(int64_t reserved)
{
    if (reserved)
        etxn_reserve(0);

    return etxn_burden();
}

int64_t hook(int64_t reserved)
{
    return etxn_burden();
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: call of etxn_burden must be preceded by call of etxn_reserve [hooks-burden-prereq]
}
