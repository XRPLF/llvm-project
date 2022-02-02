// RUN: %check_clang_tidy %s hooks-slot-limit %t

#include <stdint.h>

extern int64_t slot(uint32_t write_ptr, uint32_t write_len, uint32_t slot);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t buf[4];
    int64_t result = slot(SBUF(buf), 1);

    result = slot(SBUF(buf), -1);
// CHECK-MESSAGES: :[[@LINE-1]]:30: warning: function slot may not access more than 255 slots [hooks-slot-limit]

    return reserved;
}
