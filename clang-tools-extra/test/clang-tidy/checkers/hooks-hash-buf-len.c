// RUN: %check_clang_tidy %s hooks-hash-buf-len %t

#include <stdint.h>

extern int64_t util_sha512h(uint32_t write_ptr, uint32_t write_len,
			    uint32_t read_ptr,  uint32_t read_len);

extern int64_t rollback(uint32_t read_ptr, uint32_t read_len, int64_t error_code);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{    
    uint8_t digest[96] = { 0 };
    uint8_t hash[20];
    if (util_sha512h(SBUF(hash), SBUF(digest)) != 32)
        rollback(SBUF("Doubler: Could not compute digest for coin flip."), 4);
// CHECK-MESSAGES: :[[@LINE-2]]:22: warning: output buffer of util_sha512h needs 32 bytes for the hash [hooks-hash-buf-len]

    if (util_sha512h(0, 0, digest, sizeof(digest)) < 0)
        rollback(SBUF("Notary: Could not compute sha512 over the submitted txn."), 5);
// CHECK-MESSAGES: :[[@LINE-2]]:25: warning: output buffer of util_sha512h needs 32 bytes for the hash [hooks-hash-buf-len]

    // an error but too complicated for this check
    util_sha512h(hash, 32, digest, 96);

    return reserved;
}
