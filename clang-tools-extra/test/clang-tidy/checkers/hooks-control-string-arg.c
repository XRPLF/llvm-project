// RUN: %check_clang_tidy %s hooks-control-string-arg %t

// tests apparently define NDEBUG by default
#undef NDEBUG
#define NDEBUG

#include <stdint.h>

extern int64_t accept(uint32_t read_ptr, uint32_t read_len, int64_t error_code);

extern int64_t rollback(uint32_t read_ptr, uint32_t read_len, int64_t error_code);

int64_t hook(uint32_t reserved)
{
    accept(0, 0, 0);

    accept((uint32_t)"second", 6, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: constant string argument of accept wastes space [hooks-control-string-arg]

    return 0;
}

int64_t cbak(uint32_t reserved)
{
    // invalid but not caught by this check
    rollback(-1, -2, 0);

    // second arg also not checked
    rollback((uint32_t)"second", 60, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: constant string argument of rollback wastes space [hooks-control-string-arg]

    return 0;
}
