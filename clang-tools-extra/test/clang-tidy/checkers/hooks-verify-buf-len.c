// RUN: %check_clang_tidy %s hooks-verify-buf-len %t

#include <stdint.h>

extern int64_t util_verify(uint32_t dread_ptr, uint32_t dread_len,
			   uint32_t sread_ptr, uint32_t sread_len,
			   uint32_t kread_ptr, uint32_t kread_len);

extern int64_t rollback(uint32_t read_ptr, uint32_t read_len, int64_t error_code);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t* payload = "payload";
    uint32_t payload_len = 7;
    uint8_t signature[] =
      {
       0x30, 0x44, 0x02, 0x20, 0x39, 0x32, 0xc8, 0x92, 0xe2, 0xe5,
       0x50, 0xf3, 0xaf, 0x8e, 0xe4, 0xce, 0x9c, 0x21, 0x5a, 0x87,
       0xf9, 0xbb, 0x83, 0x1d, 0xca, 0xc8, 0x7b, 0x28, 0x38, 0xe2,
       0xc2, 0xea, 0xa8, 0x91, 0xdf, 0x0c, 0x02, 0x20, 0x30, 0xb6,
       0x1d, 0xd3, 0x65, 0x43, 0x12, 0x5d, 0x56, 0xb9, 0xf9, 0xf3,
       0xa1, 0xf9, 0x35, 0x31, 0x89, 0xe5, 0xaf, 0x33, 0xcd, 0xda,
       0x8d, 0x77, 0xa5, 0x20, 0x9a, 0xec, 0x03, 0x97, 0x8f, 0xa0,
       0x01
      };
    char ser_sign[] = "304402203932c892e2e550f3af8ee4ce9c215a87f9bb831dcac87b2838e2c2eaa891df0c022030b61dd36543125d56b9f9f3a1f9353189e5af33cdda8d77a5209aec03978fa001";
    uint8_t key[33] =
      {
       0xED, 0xDC, 0x6D, 0x9E, 0x28, 0xCA, 0x0F, 0xE2, 0xD4, 0x75,
       0xFC, 0x02, 0x1D, 0x22, 0x68, 0x81, 0x66, 0x6E, 0xA1, 0x06,
       0xFB, 0xD2, 0x22, 0x2C, 0x8C, 0x21, 0x10, 0x36, 0x8A, 0x49,
       0xC9, 0x51, 0x3C
      };

    if (!util_verify(payload, payload_len, signature, sizeof(signature), key, sizeof(key)))
        rollback(SBUF("Blacklist: Invalid signature in memo."), 60);

    // switched
    if (!util_verify(payload, payload_len, key, sizeof(key), signature, sizeof(signature)))
// CHECK-MESSAGES: :[[@LINE-1]]:73: warning: key of util_verify must have 33 bytes [hooks-verify-buf-len]
        rollback(SBUF("Blacklist: Invalid signature in memo."), 60);

    // wrong format
    if (!util_verify(payload, payload_len, ser_sign, sizeof(ser_sign), key, sizeof(key)))
// CHECK-MESSAGES: :[[@LINE-1]]:54: warning: signature of util_verify must have between 8 and 72 bytes [hooks-verify-buf-len]
        rollback(SBUF("Blacklist: Invalid signature in memo."), 60);

    return reserved;
}
