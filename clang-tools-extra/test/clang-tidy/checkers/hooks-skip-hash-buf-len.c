// RUN: %check_clang_tidy %s hooks-skip-hash-buf-len %t

#include <stdint.h>

extern int64_t hook_skip(uint32_t read_ptr,  uint32_t read_len, uint32_t flags);

int64_t hook(uint32_t reserved)
{
    uint8_t phash[] = { 0x19U, 0xFEU, 0x69U, 0xF1U, 0x53U, 0x66U, 0x4EU, 0x8CU, 
                        0x97U, 0xF4U, 0x4CU, 0x5CU, 0x3CU, 0x65U, 0x63U, 0x79U, 
                        0xC2U, 0xD0U, 0x26U, 0xE7U, 0x90U, 0xEFU, 0x38U, 0xF7U, 
                        0xEDU, 0x73U, 0xE9U, 0xCEU, 0x9CU, 0x9DU, 0xBFU, 0x03U };
    int64_t result = hook_skip(phash, 3, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:39: warning: hash buffer of hook_skip must have 32 bytes [hooks-skip-hash-buf-len]

    hook_skip(phash, 32, 2);

    return result;
}
