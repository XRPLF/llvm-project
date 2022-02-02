// RUN: %check_clang_tidy %s hooks-slot-keylet-buf-len %t

#include <stdint.h>

// declared differently in hookapi.h - this declaration is from documentation
extern int64_t slot_set(uint32_t read_ptr, uint32_t read_len, uint32_t slot);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t keylet[34] = { 0 };

    int64_t slot_no = slot_set(SBUF(keylet), 0);

    slot_set(keylet, 33, -1);
// CHECK-MESSAGES: :[[@LINE-1]]:22: warning: input buffer of slot_set must have either 32 or 34 bytes [hooks-slot-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:26: warning: function slot_set may not access more than 255 slots [hooks-slot-keylet-buf-len]

    return reserved;
}
