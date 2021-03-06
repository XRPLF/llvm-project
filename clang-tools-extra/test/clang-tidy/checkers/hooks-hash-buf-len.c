// RUN: %check_clang_tidy %s hooks-hash-buf-len %t

#include <stdint.h>

extern int64_t etxn_nonce(uint32_t write_ptr,  uint32_t write_len);

extern int64_t ledger_nonce(uint32_t write_ptr, uint32_t write_len);

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

    // an error, but too complicated for this check
    hook_hash(hash, 32);

    hook_hash(hash, 3);
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: output buffer of hook_hash needs 32 bytes for the hash [hooks-hash-buf-len]

    ledger_last_hash(SBUF(hash));
// CHECK-MESSAGES: :[[@LINE-1]]:22: warning: output buffer of ledger_last_hash needs 32 bytes for the hash [hooks-hash-buf-len]

    etxn_nonce(SBUF(digest));

    ledger_nonce(SBUF(hash));
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: output buffer of ledger_nonce needs 32 bytes for the hash [hooks-hash-buf-len]

    return reserved;
}
