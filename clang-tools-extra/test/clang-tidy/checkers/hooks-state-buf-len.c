// RUN: %check_clang_tidy %s hooks-state-buf-len %t

#include <stdint.h>

extern int64_t state(uint32_t write_ptr, uint32_t write_len, uint32_t kread_ptr, uint32_t kread_len);

extern int64_t state_set(uint32_t read_ptr, uint32_t read_len, uint32_t kread_ptr, uint32_t kread_len);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t digest[96];
    uint8_t hash[32];
    uint8_t key[3] = { 0 };
    uint8_t key2[32] = { 0 };

    state(digest + 32, 32, SBUF(key));
// CHECK-MESSAGES: :[[@LINE-1]]:28: warning: key buffer of function state should have 32 bytes [hooks-state-buf-len]

    if (state_set(SBUF(hash), SBUF(key)) != 32)
// CHECK-MESSAGES: :[[@LINE-1]]:31: warning: key buffer of function state_set should have 32 bytes [hooks-state-buf-len]
	return 0;

    state_set(SBUF(hash), SBUF(key2));

    return reserved;
}
