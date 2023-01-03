// RUN: %check_clang_tidy %s hooks-guard-call-non-const %t

#include "stdint.h"

extern int32_t _g (uint32_t id, uint32_t maxiter);
extern int64_t trace_num (const char *read_ptr, uint32_t read_len, int64_t number);

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(int64_t reserved)
{
    int w = 3;
    for (int i = 0; GUARD(w), i < w; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: 'GUARD' calls can only have const integers or literals as an argument [hooks-guard-call-non-const]
    {
        trace_num("one", 3, i);
    }

    const int v = 4;
    for (int i = 0; GUARD(v), i < v; ++i)
    {
        for (int j = 0; GUARD(reserved), j < 3; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: 'GUARD' calls can only have const integers or literals as an argument [hooks-guard-call-non-const]
        {
            trace_num("two", 3, j);
        }
    }

    return 0;
}
