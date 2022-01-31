// RUN: %check_clang_tidy %s hooks-float-one-pure %t

#include <stdint.h>

extern int64_t float_one();

int64_t hook(int64_t reserved)
{
    return float_one();
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: output of float_one is constant [hooks-float-one-pure]
// CHECK-FIXES: return 0x54838d7ea4c68000ll;
}
