// RUN: %check_clang_tidy %s hooks-slot-limit %t

#include <stdint.h>

extern int64_t slot(uint32_t write_ptr, uint32_t write_len, uint32_t slot);

extern int64_t slot_clear(uint32_t slot);

extern int64_t slot_count(uint32_t slot);

extern int64_t slot_size(uint32_t slot);

extern int64_t slot_float(uint32_t slot);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t buf[4];
    int64_t result = slot(SBUF(buf), 1);

    result = slot(SBUF(buf), -1);
// CHECK-MESSAGES: :[[@LINE-1]]:30: warning: function slot may not access more than 255 slots [hooks-slot-limit]

    slot_clear(1000);
// CHECK-MESSAGES: :[[@LINE-1]]:16: warning: function slot_clear may not access more than 255 slots [hooks-slot-limit]

    slot_count(1);

    slot_size(0);
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: function slot_size may not access more than 255 slots [hooks-slot-limit]

    slot_float(sizeof(buf));

    return reserved;
}
