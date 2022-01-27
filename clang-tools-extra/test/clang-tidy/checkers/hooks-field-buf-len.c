// RUN: %check_clang_tidy %s hooks-field-buf-len %t

#include <stdint.h>

extern int64_t sto_subfield(uint32_t read_ptr, uint32_t read_len, uint32_t field_id);

int64_t hook(int64_t reserved)
{
    uint8_t memos[2048] = { 0 };
    int64_t memos_len = sizeof(memos);
    int64_t memo_lookup = sto_subfield(memos, memos_len, 0);

    sto_subfield(0, 0, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: input buffer of sto_subfield must not be empty [hooks-field-buf-len]
}
