// RUN: %check_clang_tidy %s hooks-emit-buf-len %t

#include <stdint.h>

extern int64_t emit(uint32_t write_ptr, uint32_t write_len, uint32_t read_ptr, uint32_t read_len);

int64_t hook(int64_t reserved)
{
    uint8_t emithash[16];
    uint8_t buffer[1024] = { 0 };
    emit(emithash, sizeof(emithash), buffer, sizeof(buffer));
// CHECK-MESSAGES: :[[@LINE-1]]:20: warning: output buffer of emit needs 32 bytes for transaction hash [hooks-emit-buf-len]

    uint8_t emithash2[32];
    emit(emithash2, sizeof(emithash2), buffer, sizeof(buffer));
}
