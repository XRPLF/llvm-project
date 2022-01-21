// RUN: %check_clang_tidy %s hooks-account-buf-len %t

#include <stdint.h>

extern int64_t hook_account(uint32_t write_ptr,  uint32_t write_len);

extern int64_t rollback(int64_t error_code);

int64_t hook(int64_t reserved)
{    
    uint8_t hook_accid[10];
    if (hook_account((uint32_t)(hook_accid), sizeof(hook_accid)) < 0)
        rollback(1);
// CHECK-MESSAGES: :[[@LINE-2]]:46: warning: output buffer of hook_account needs 20 bytes for account ID [hooks-account-buf-len]

    // an error but for a different check...
    uint8_t hook_accid2[20];
    hook_account((uint32_t)(hook_accid), sizeof(hook_accid2));

    return reserved;
}
