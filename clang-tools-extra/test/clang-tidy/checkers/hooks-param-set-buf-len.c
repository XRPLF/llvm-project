// RUN: %check_clang_tidy %s hooks-param-set-buf-len %t

#include <stdint.h>

extern int64_t hook_param_set(uint32_t read_ptr, uint32_t read_len, uint32_t kread_ptr, uint32_t kread_len, uint32_t hread_ptr, uint32_t hread_len);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(uint32_t reserved)
{
    uint8_t pvalue[] = "some parameter value";
    uint8_t pname[] = "paramname";
    uint8_t phash[] = { 0x19U, 0xFEU, 0x69U, 0xF1U, 0x53U, 0x66U, 0x4EU, 0x8CU, 
                        0x97U, 0xF4U, 0x4CU, 0x5CU, 0x3CU, 0x65U, 0x63U, 0x79U, 
                        0xC2U, 0xD0U, 0x26U, 0xE7U, 0x90U, 0xEFU, 0x38U, 0xF7U, 
                        0xEDU, 0x73U, 0xE9U, 0xCEU, 0x9CU, 0x9DU, 0xBFU, 0x03U };
    uint8_t large[512] = { 0 };

    int64_t result = hook_param_set(pvalue, 0, pname, 0, phash, 0);
// CHECK-MESSAGES: :[[@LINE-1]]:55: warning: name buffer of function hook_param_set must have between 1 and 32 bytes [hooks-param-set-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:65: warning: hash buffer of function hook_param_set must have 32 bytes [hooks-param-set-buf-len]
    if (result)
        return result;

    hook_param_set(SBUF(large), pname, sizeof(pname), phash, sizeof(phash));
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: value buffer of function hook_param_set can have at most 128 bytes [hooks-param-set-buf-len]

    return reserved;
}
