// RUN: %check_clang_tidy %s hooks-float-manip-pure %t

#include <stdint.h>

extern int64_t float_one();

extern int64_t float_exponent_set(int64_t float1, int32_t exponent);

extern int64_t float_mantissa_set(int64_t float1, int64_t mantissa);

extern int64_t float_sign_set(int64_t float1, uint32_t negative);

int64_t hook(int64_t reserved)
{
    int64_t new_xfl = float_exponent_set(0x54871afd498d0000ll, -71);
// CHECK-MESSAGES: :[[@LINE-1]]:23: warning: output of float_exponent_set can be precomputed [hooks-float-manip-pure]
// CHECK-FIXES: trace_num((uint32_t)"new_xfl", sizeof("new_xfl"), (int64_t)new_xfl);

    new_xfl = float_mantissa_set(0x54871afd498d0000ll, 100000000000000ULL);
// CHECK-MESSAGES: :[[@LINE-1]]:56: warning: last argument of float_mantissa_set must be between 1000000000000000 and 9999999999999999 [hooks-float-manip-pure]

    int64_t negative_one = float_sign_set(float_one(), -1);
// CHECK-MESSAGES: :[[@LINE-1]]:56: warning: last argument of float_sign_set must be between 0 and 1 [hooks-float-manip-pure]

    return reserved;
}
