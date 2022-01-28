// RUN: %check_clang_tidy %s hooks-detail-buf-len %t

#include <stdint.h>

extern int64_t etxn_details(uint32_t write_ptr, uint32_t write_len);

int64_t hook(int64_t reserved)
{    
    uint8_t emitdet[105];
    int64_t result = etxn_details(emitdet, 105);

    etxn_details(emitdet, 10);
// CHECK-MESSAGES: :[[@LINE-1]]:27: warning: output buffer of etxn_details needs 105 bytes for emit details [hooks-detail-buf-len]

    return result;
}
