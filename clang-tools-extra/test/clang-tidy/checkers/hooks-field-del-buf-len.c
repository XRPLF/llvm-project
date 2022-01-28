// RUN: %check_clang_tidy %s hooks-field-del-buf-len %t

#include <stdint.h>

extern int64_t sto_erase(uint32_t write_ptr, uint32_t write_len,
			 uint32_t read_ptr, uint32_t read_len, uint32_t field_id);

#define sfSigners 0xf0003UL

#define MAX_MEMO_SIZE 4096

int64_t hook(int64_t reserved)
{
    uint8_t buffer[4096];
    uint8_t memos[2048] = { 0 };

    int64_t result = sto_erase(buffer, MAX_MEMO_SIZE, memos, sizeof(memos), sfSigners);

    sto_erase(buffer, sizeof(memos), memos, MAX_MEMO_SIZE, sfSigners);
// CHECK-MESSAGES: :[[@LINE-1]]:23: warning: output buffer of sto_erase too small for input object [hooks-field-del-buf-len]

    sto_erase(buffer, 20480, memos, 20480, sfSigners);
// CHECK-MESSAGES: :[[@LINE-1]]:37: warning: input object of sto_erase can have at most 16384 bytes [hooks-field-del-buf-len]
}
