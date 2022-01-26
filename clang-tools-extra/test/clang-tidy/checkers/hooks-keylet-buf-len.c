// RUN: %check_clang_tidy %s hooks-keylet-buf-len %t

#include <stdint.h>

extern int64_t util_keylet(uint32_t write_ptr, uint32_t write_len, uint32_t keylet_type,
			   uint32_t a, uint32_t b, uint32_t c,
			   uint32_t d, uint32_t e, uint32_t f);

extern int64_t rollback(uint32_t read_ptr, uint32_t read_len, int64_t error_code);

#define SBUF(str) (uint32_t)(str), sizeof(str)

#define KEYLET_HOOK 1
#define KEYLET_HOOK_STATE 2
#define KEYLET_ACCOUNT 3
#define KEYLET_AMENDMENTS 4
#define KEYLET_CHILD 5
#define KEYLET_SKIP 6
#define KEYLET_FEES 7
#define KEYLET_NEGATIVE_UNL 8
#define KEYLET_LINE 9
#define KEYLET_OFFER 10
#define KEYLET_QUALITY 11
#define KEYLET_EMITTED_DIR 12
#define KEYLET_TICKET 13
#define KEYLET_SIGNERS 14
#define KEYLET_CHECK 15
#define KEYLET_DEPOSIT_PREAUTH 16
#define KEYLET_UNCHECKED 17
#define KEYLET_OWNER_DIR 18
#define KEYLET_PAGE 19
#define KEYLET_ESCROW 20
#define KEYLET_PAYCHAN 21
#define KEYLET_EMITTED 22

int64_t hook(int64_t reserved)
{
    uint8_t keylet[34];
    uint8_t hook_accid[20] = { 0 };
    uint8_t account_field[20] = { 0 };
    uint8_t currency_code[20] = { 0 };
    if (util_keylet(keylet, 34, KEYLET_LINE, hook_accid, 20,
		    account_field, 20, currency_code, 20) != 34)
        rollback(SBUF("Keylet Failed."), 1);

    util_keylet(keylet, 20, KEYLET_LINE, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:25: warning: output buffer of util_keylet needs 34 bytes for the keylet [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), 999999, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:41: warning: invalid keylet type 999999 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_HOOK, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_HOOK' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_HOOK' must be 0 [hooks-keylet-buf-len]

    // avoid false positives
    uint32_t zero = reserved;
    util_keylet(keylet, sizeof(keylet), KEYLET_HOOK, hook_accid, 20,
		zero, zero, zero, zero);

    util_keylet(keylet, sizeof(keylet), KEYLET_HOOK_STATE, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: state key of keylet type 'KEYLET_HOOK_STATE' must have 32 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 2 generic parameters of keylet type 'KEYLET_HOOK_STATE' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_AMENDMENTS, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:72: warning: generic parameters of keylet type 'KEYLET_AMENDMENTS' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: generic parameters of keylet type 'KEYLET_AMENDMENTS' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: generic parameters of keylet type 'KEYLET_AMENDMENTS' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_CHILD, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:67: warning: key of keylet type 'KEYLET_CHILD' must have 32 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_CHILD' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_CHILD' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_SKIP, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_SKIP' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_SKIP' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_FEES, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:66: warning: generic parameters of keylet type 'KEYLET_FEES' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: generic parameters of keylet type 'KEYLET_FEES' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: generic parameters of keylet type 'KEYLET_FEES' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_NEGATIVE_UNL, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:74: warning: generic parameters of keylet type 'KEYLET_NEGATIVE_UNL' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: generic parameters of keylet type 'KEYLET_NEGATIVE_UNL' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: generic parameters of keylet type 'KEYLET_NEGATIVE_UNL' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_LINE, 0, 1, 2, 3, 4, 5);
// CHECK-MESSAGES: :[[@LINE-1]]:57: warning: high account ID of keylet type 'KEYLET_LINE' must have 20 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:63: warning: low account ID of keylet type 'KEYLET_LINE' must have 20 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:69: warning: currency code of keylet type 'KEYLET_LINE' must have 20 bytes [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_OFFER, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 3 generic parameters of keylet type 'KEYLET_OFFER' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 3 generic parameters of keylet type 'KEYLET_OFFER' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_QUALITY, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:69: warning: keylet of keylet type 'KEYLET_QUALITY' must have 34 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 2 generic parameters of keylet type 'KEYLET_QUALITY' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_EMITTED_DIR, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:73: warning: generic parameters of keylet type 'KEYLET_EMITTED_DIR' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: generic parameters of keylet type 'KEYLET_EMITTED_DIR' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: generic parameters of keylet type 'KEYLET_EMITTED_DIR' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_SIGNERS, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_SIGNERS' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_SIGNERS' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_CHECK, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 3 generic parameters of keylet type 'KEYLET_CHECK' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 3 generic parameters of keylet type 'KEYLET_CHECK' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_DEPOSIT_PREAUTH, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:37: warning: last 2 generic parameters of keylet type 'KEYLET_DEPOSIT_PREAUTH' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_UNCHECKED, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:71: warning: key of keylet type 'KEYLET_UNCHECKED' must have 32 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_UNCHECKED' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_UNCHECKED' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_OWNER_DIR, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_OWNER_DIR' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_OWNER_DIR' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_PAGE, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:66: warning: key of keylet type 'KEYLET_PAGE' must have 32 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 2 generic parameters of keylet type 'KEYLET_PAGE' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_ESCROW, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:18: warning: last 3 generic parameters of keylet type 'KEYLET_ESCROW' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:37: warning: last 3 generic parameters of keylet type 'KEYLET_ESCROW' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_PAYCHAN, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:37: warning: last generic parameter of keylet type 'KEYLET_PAYCHAN' must be 0 [hooks-keylet-buf-len]

    util_keylet(keylet, sizeof(keylet), KEYLET_EMITTED, hook_accid, 20,
		account_field, 20, currency_code, 20);
// CHECK-MESSAGES: :[[@LINE-2]]:69: warning: key of keylet type 'KEYLET_EMITTED' must have 32 bytes [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-2]]:18: warning: last 4 generic parameters of keylet type 'KEYLET_EMITTED' must be 0 [hooks-keylet-buf-len]
// CHECK-MESSAGES: :[[@LINE-3]]:37: warning: last 4 generic parameters of keylet type 'KEYLET_EMITTED' must be 0 [hooks-keylet-buf-len]

    return reserved;
}
