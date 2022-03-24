// RUN: %check_clang_tidy %s hooks-param-buf-len %t

#include <stdint.h>

extern int64_t hook_param(uint32_t write_ptr, uint32_t write_len, uint32_t read_ptr, uint32_t read_len);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(uint32_t reserved)
{
    uint8_t pname[] = {0xCAU, 0xFEU};
    uint8_t pvalue[32];
    int64_t value_len = hook_param((uint32_t)pvalue, 32, (uint32_t)pname, 0);  
// CHECK-MESSAGES: :[[@LINE-1]]:75: warning: name buffer of function hook_param must have between 1 and 32 bytes [hooks-param-buf-len]

    hook_param((uint32_t)pvalue, 3, (uint32_t)pname, 2);
// CHECK-MESSAGES: :[[@LINE-1]]:34: warning: value buffer of function hook_param must have at least 32 bytes [hooks-param-buf-len]

    return reserved;
}
