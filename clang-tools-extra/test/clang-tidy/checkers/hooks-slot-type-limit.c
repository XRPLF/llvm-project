// RUN: %check_clang_tidy %s hooks-slot-type-limit %t

#include <stdint.h>

extern int64_t slot_type(uint32_t slot, uint32_t flags);

int64_t hook(int64_t reserved)
{
    int64_t amt_slot = 0, is_xrp = slot_type(amt_slot, 2);
// CHECK-MESSAGES: :[[@LINE-1]]:46: warning: function slot_type accepts only 0 or 1 flags [hooks-slot-type-limit]

    slot_type(0, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: function slot_type may not access more than 255 slots [hooks-slot-type-limit]

    return reserved;
}
