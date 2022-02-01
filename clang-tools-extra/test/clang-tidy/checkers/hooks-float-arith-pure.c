// RUN: %check_clang_tidy %s hooks-float-arith-pure %t

#include <stdint.h>

extern int64_t float_multiply(int64_t float1, int64_t float2);

extern int64_t float_mulratio(int64_t float1, uint32_t round_up, uint32_t numerator, uint32_t denominator);

extern int64_t float_negate(int64_t float1);

extern int64_t float_compare(int64_t float1, int64_t float2, uint32_t mode );

extern int64_t float_sum(int64_t float1, int64_t float2);

extern int64_t float_invert(int64_t float1);

extern int64_t float_divide(int64_t float1, int64_t float2);

extern int64_t float_exponent(int64_t float1);

extern int64_t float_sign(int64_t float1);

extern int64_t float_int(int64_t float1, uint32_t decimal_places, uint32_t abs);

#define LIQ_COLLATERALIZATION_NUMERATOR 5
#define LIQ_COLLATERALIZATION_DENOMINATOR 6

int64_t hook(int64_t reserved)
{
    int64_t exchange_rate = 0x54871afd498d0000ll;
    int64_t vault_xrp = 0x548aa87bee538000ll;

    // constant tracking in hook checks is not very advanced...
    int64_t max_vault_pusd = float_multiply(vault_xrp, exchange_rate);

    int64_t max_vault_pusd2 = float_multiply(0x54871afd498d0000ll, 0x548aa87bee538000ll);
// CHECK-MESSAGES: :[[@LINE-1]]:31: warning: output of float_multiply can be precomputed [hooks-float-arith-pure]

    int64_t required_vault_xrp;
    required_vault_xrp = float_divide(0x548e35fa931a0000ll, 0x54871afd498d0000ll);
// CHECK-MESSAGES: :[[@LINE-1]]:26: warning: output of float_divide can be precomputed [hooks-float-arith-pure]

    required_vault_xrp = float_divide(0x548e35fa931a0000ll, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:61: warning: function float_divide will fail division by 0 [hooks-float-arith-pure]

    required_vault_xrp = float_mulratio(vault_xrp, 0, LIQ_COLLATERALIZATION_DENOMINATOR, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:90: warning: function float_mulratio will fail division by 0 [hooks-float-arith-pure]

    required_vault_xrp = float_mulratio(0x548e35fa931a0000ll, 0, LIQ_COLLATERALIZATION_DENOMINATOR, LIQ_COLLATERALIZATION_NUMERATOR);
// CHECK-MESSAGES: :[[@LINE-1]]:26: warning: output of float_mulratio can be precomputed [hooks-float-arith-pure]

    int64_t pusd_to_send = float_sum(max_vault_pusd, float_negate(0));
// CHECK-MESSAGES: :[[@LINE-1]]:54: warning: output of float_negate can be precomputed [hooks-float-a

    int64_t zero = float_negate(0);
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: output of float_negate can be precomputed [hooks-float-arith-pure]
// CHECK-FIXES: trace_num((uint32_t)"zero", sizeof("zero"), (int64_t)zero);

    int64_t one = float_invert(0x54838d7ea4c68000ll);
// CHECK-MESSAGES: :[[@LINE-1]]:19: warning: output of float_invert can be precomputed [hooks-float-arith-pure]
// CHECK-FIXES: trace_num((uint32_t)"one", sizeof("one"), (int64_t)one);

    int64_t exponent = float_exponent(0x54838d7ea4c68000ll);
// CHECK-MESSAGES: :[[@LINE-1]]:24: warning: output of float_exponent can be precomputed [hooks-float-arith-pure]

    int64_t sign = float_sign(zero);

    return zero + one + max_vault_pusd * max_vault_pusd2;
}
