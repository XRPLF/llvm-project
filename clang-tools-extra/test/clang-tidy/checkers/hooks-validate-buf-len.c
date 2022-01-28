// RUN: %check_clang_tidy %s hooks-validate-buf-len %t

#include <stdint.h>

extern int64_t sto_validate(uint32_t read_ptr, uint32_t read_len);

int64_t hook(int64_t reserved)
{
    uint8_t memos[2048] = { 0 };
    int64_t memos_len = sizeof(memos);
    int64_t memo_lookup = sto_validate(memos, memos_len);

    sto_validate(0, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: input buffer of sto_validate must not be empty [hooks-validate-buf-len]
}
