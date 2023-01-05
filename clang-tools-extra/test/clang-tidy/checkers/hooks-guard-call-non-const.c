// RUN: %check_clang_tidy %s hooks-guard-call-non-const %t

#include "stdint.h"

extern int32_t _g (uint32_t id, uint32_t maxiter);
extern int64_t trace_num (const char *read_ptr, uint32_t read_len, int64_t number);

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(int64_t reserved)
{
    int w = 3;
    for (int i = 0; GUARD(w), i < w; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: 'GUARD' calls can only have compile-time constant as an argument [hooks-guard-call-non-const]
    {
        trace_num("one", 3, i);
    }

    const int v = 4;
    for (int i = 0; GUARD(v), i < v; ++i)
    {
        for (int j = 0; GUARD(reserved), j < 3; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: 'GUARD' calls can only have compile-time constant as an argument [hooks-guard-call-non-const]
        {
            trace_num("two", 3, j);
        }
    }

    for (int i = 0; GUARD(v + 2 + 1), i < 4; ++i)
    {
        trace_num("three", 5, i);
    }

    for (int i = 0; GUARD(v + 2 + 1 * w), i < 4; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: 'GUARD' calls can only have compile-time constant as an argument [hooks-guard-call-non-const]
    {
        trace_num("four", 4, i);
    }

    for (int i = 0; _g(1 + w, 3), i < 3; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:24: warning: 'GUARD' call guard ID argument must be compile-time constant [hooks-guard-call-non-const]
    {
        trace_num("five", 4, i);
    }
    
    for (int i = 0; _g(1 + 2, 3), i < 3; ++i)
    {
        trace_num("six", 3, i);
    }

    for (int i = 0; _g(3, 3), i < 3; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:24: warning: 'GUARD' call guard ID argument must be unique [hooks-guard-call-non-const]
    {
        trace_num("seven", 5, i);
    }

    return 0;
}
