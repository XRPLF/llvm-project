// RUN: %check_clang_tidy %s hooks-transaction-hash-buf-len %t

#include <stdint.h>

extern int64_t otxn_id(uint32_t write_ptr, uint32_t write_len);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t buf[4];
    return otxn_id(SBUF(buf));
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: output buffer of otxn_id needs 32 bytes for the hash [hooks-transaction-hash-buf-len]
}
