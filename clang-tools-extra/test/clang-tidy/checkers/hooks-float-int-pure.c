// RUN: %check_clang_tidy %s hooks-float-int-pure %t

#include <stdint.h>

extern int64_t float_int(int64_t float1, uint32_t decimal_places, uint32_t abs);

int64_t hook(int64_t reserved)
{
    int64_t xrp_to_send = reserved;

    int64_t a = float_int(0, 6, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:17: warning: output of float_int can be precomputed [hooks-float-int-pure]

    a = float_int(xrp_to_send, 6, 0);

    a = float_int(xrp_to_send, 60, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:32: warning: decimal places of float_int can be at most 15 [hooks-float-int-pure]

    return a;
}
