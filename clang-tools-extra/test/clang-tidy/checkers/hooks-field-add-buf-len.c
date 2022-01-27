// RUN: %check_clang_tidy %s hooks-field-add-buf-len %t

#include <stdint.h>

extern int64_t sto_emplace(uint32_t write_ptr, uint32_t write_len,
			   uint32_t sread_ptr, uint32_t sread_len,
			   uint32_t fread_ptr, uint32_t fread_len, uint32_t field_id);

#define sfSequence 0x20004UL

int64_t hook(int64_t reserved)
{
    uint8_t buffer[4096];
    uint8_t memos[2048] = { 0 };
    uint8_t zeroed[6] = { 0 };
    int64_t tx_len = sto_emplace(buffer, sizeof(buffer), memos, sizeof(memos),
				 zeroed, 5, sfSequence);

    sto_emplace(0, 0, memos, sizeof(memos), zeroed, 5, sfSequence);
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: output buffer of sto_emplace too small for input object + input field [hooks-field-add-buf-len]

    sto_emplace(buffer, sizeof(memos), memos, sizeof(memos), zeroed, 5, sfSequence);    
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: output buffer of sto_emplace too small for input object + input field [hooks-field-add-buf-len]

    sto_emplace(buffer, sizeof(buffer), memos, sizeof(memos), zeroed, sizeof(buffer) + sizeof(memos), sfSequence);    
// CHECK-MESSAGES: :[[@LINE-1]]:71: warning: input field of sto_emplace can have at most 4096 bytes [hooks-field-add-buf-len]
}
