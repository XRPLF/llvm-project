// RUN: %check_clang_tidy %s hooks-detail-prereq %t

#include <stdint.h>

extern int64_t etxn_details(uint32_t write_ptr, uint32_t write_len);

extern int64_t etxn_reserve(uint32_t count);

// warning not emitted if the prerequisite _could_ be called - that's
// on the programmer (and run-time checks in rippled)...
int64_t hook2(int64_t reserved)
{
    uint8_t emitdet[105];

    if (reserved)
        etxn_reserve(0);

    return etxn_details(emitdet, 105);
}

int64_t hook(int64_t reserved)
{
    uint8_t emitdet[105];
    return etxn_details(emitdet, 105);
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: call of etxn_details must be preceded by call of etxn_reserve [hooks-detail-prereq]
}
