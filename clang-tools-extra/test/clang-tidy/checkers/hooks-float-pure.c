// RUN: %check_clang_tidy %s hooks-float-pure %t

#include <stdint.h>

extern int64_t float_set(int32_t exponent, int64_t mantissa);

int64_t cbak(int64_t reserved)
{
    int64_t r = float_set(0, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:17: warning: float_set(0, 0) is 0 [hooks-float-pure]
// CHECK-FIXES: int64_t r = 0;

    // just return doesn't match, which maybe it should; for the
    // special case of direct replacement it even could, but inserting
    // a trace statement is problematic after return...
    return float_set(0, 0);
}

int64_t hook(int64_t reserved)
{
    // unused return is a different test
    float_set(reserved, 1);

    int64_t required_limit = float_set(10, 1);
// CHECK-MESSAGES: :[[@LINE-1]]:30: warning: output of float_set can be precomputed [hooks-float-pure]

    required_limit = float_set(10, 1);
// CHECK-MESSAGES: :[[@LINE-1]]:22: warning: output of float_set can be precomputed [hooks-float-pure]

    return required_limit;
}
