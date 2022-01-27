// RUN: %check_clang_tidy %s hooks-array-buf-len %t

#include <stdint.h>

extern int64_t sto_subarray(uint32_t read_ptr, uint32_t read_len, uint32_t array_id);

int64_t hook(int64_t reserved)
{
    uint8_t memos[2048] = { 0 };
    int64_t memos_len = sizeof(memos);
    int64_t memo_lookup = sto_subarray(memos, memos_len, 0);

    sto_subarray(0, 0, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: input buffer of sto_subarray must not be empty [hooks-array-buf-len]

    memo_lookup = sto_subarray(memos, memos_len, 10000);
// CHECK-MESSAGES: :[[@LINE-1]]:50: warning: array index of sto_subarray can be at most 1023 [hooks-array-buf-len]
}
