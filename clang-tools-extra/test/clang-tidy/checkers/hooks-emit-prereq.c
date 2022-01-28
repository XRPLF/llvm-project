// RUN: %check_clang_tidy %s hooks-emit-prereq %t

#include <stdint.h>

extern int64_t emit(uint32_t write_ptr, uint32_t write_len, uint32_t read_ptr, uint32_t read_len);

extern int64_t etxn_reserve(uint32_t count);

int64_t hook(int64_t reserved)
{
    uint8_t emithash[32];
    uint8_t buffer[1024] = { 0 };
    return emit(emithash, sizeof(emithash), buffer, sizeof(buffer));
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: call of emit must be preceded by call of etxn_reserve [hooks-emit-prereq]
}

int64_t hook2(int64_t reserved)
{
    uint8_t emithash[32];
    uint8_t buffer[1024] = { 0 };

    if (reserved)
        etxn_reserve(1);

    return emit(emithash, sizeof(emithash), buffer, sizeof(buffer));
}
