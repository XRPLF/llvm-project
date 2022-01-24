// RUN: %check_clang_tidy %s hooks-account-conv-buf-len %t

#include <stdint.h>

extern int64_t util_raddr(uint32_t write_ptr, uint32_t write_len,
			  uint32_t read_ptr,  uint32_t read_len);

int64_t hook(int64_t reserved)
{
  uint8_t raddr_out[40];
  uint8_t acc_id[20] =
    {
     0x2dU, 0xd8U, 0xaaU, 0xdbU, 0x4eU, 0x15U,               
     0xebU, 0xeaU,  0xeU, 0xfdU, 0x78U, 0xd1U, 0xb0U,
     0x35U, 0x91U,  0x4U, 0x7bU, 0xfaU, 0x1eU,  0xeU
    };
  int64_t bytes_written = 
    util_raddr(raddr_out, sizeof(raddr_out), acc_id, 10);
// CHECK-MESSAGES: :[[@LINE-1]]:54: warning: account ID of util_raddr must have 20 bytes [hooks-account-conv-buf-len]

  util_raddr(raddr_out, sizeof(raddr_out), acc_id, 20);

  util_raddr(raddr_out, sizeof(raddr_out), acc_id, 30);
// CHECK-MESSAGES: :[[@LINE-1]]:52: warning: account ID of util_raddr must have 20 bytes [hooks-account-conv-buf-len]
  
  return reserved;
}
