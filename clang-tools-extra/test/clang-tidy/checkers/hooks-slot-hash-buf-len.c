// RUN: %check_clang_tidy %s hooks-slot-hash-buf-len %t

#include <stdint.h>

// declared differently in hookapi.h - this declaration is from documentation
int64_t slot_id(uint32_t write_ptr, uint32_t write_len, uint32_t slot_no);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t buf[4];
    return slot_id(SBUF(buf), -1);
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: output buffer of slot_id needs 32 bytes for the hash [hooks-slot-hash-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:31: warning: function slot_id may not access more than 255 slots [hooks-slot-hash-buf-len]
}
