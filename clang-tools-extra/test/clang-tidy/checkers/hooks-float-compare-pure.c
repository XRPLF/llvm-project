// RUN: %check_clang_tidy %s hooks-float-compare-pure %t

#include <stdint.h>

extern int64_t float_compare(int64_t float1, int64_t float2, uint32_t mode);

extern int64_t rollback(uint32_t read_ptr, uint32_t read_len, int64_t error_code);

#define SBUF(str) (uint32_t)(str), sizeof(str)

#define COMPARE_EQUAL 1U
#define COMPARE_LESS 2U
#define COMPARE_GREATER 4U

int64_t hook(int64_t reserved)
{
    int64_t user_trustline_limit = reserved;
    int64_t required_limit = reserved + 1;

    if (float_compare(user_trustline_limit, required_limit, COMPARE_EQUAL | COMPARE_GREATER) != 1)
        rollback(SBUF("Peggy: You must set a trustline for USD to peggy for limit of at least 10B"), 1);

    if (float_compare(user_trustline_limit, required_limit, -1) != 1)
// CHECK-MESSAGES: :[[@LINE-1]]:61: warning: function float_compare has invalid mode [hooks-float-compare-pure]
        rollback(SBUF("Peggy: You must set a trustline for USD to peggy for limit of at least 10B"), 2);

    if (float_compare(0x54871afd498d0000ll, 0x548aa87bee538000ll, COMPARE_EQUAL | COMPARE_GREATER) != 1)
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: output of float_compare can be precomputed [hooks-float-compare-pure]
        rollback(SBUF("Peggy: You must set a trustline for USD to peggy for limit of at least 10B"), 3);

    return 0;
}
