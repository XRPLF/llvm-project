// RUN: %check_clang_tidy %s hooks-slot-sub-limit %t

#include <stdint.h>

extern int64_t slot_subarray(uint32_t parent_slot, uint32_t array_id, uint32_t new_slot);

extern int64_t slot_subfield(uint32_t parent_slot, uint32_t field_id, uint32_t new_slot);

#define sfAmount 0x60001UL

int64_t hook(int64_t reserved)
{
    int slot_no = 1, subslot = 0, i = 0;
    subslot = slot_subarray(slot_no, i, subslot);

    slot_subarray(slot_no, i, 999);
// CHECK-MESSAGES: :[[@LINE-1]]:31: warning: function slot_subarray may not access more than 255 slots [hooks-slot-sub-limit]

    slot_subfield(0, sfAmount, subslot);
// CHECK-MESSAGES: :[[@LINE-1]]:19: warning: function slot_subfield may not access more than 255 slots [hooks-slot-sub-limit]

    return reserved;
}
