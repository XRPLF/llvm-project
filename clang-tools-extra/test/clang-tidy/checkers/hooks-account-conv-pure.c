// RUN: %check_clang_tidy %s hooks-account-conv-pure %t

#include <stdint.h>

int64_t util_raddr(
    uint32_t write_ptr,
    uint32_t write_len,
    uint32_t read_ptr,
    uint32_t read_len);

int64_t hook(int64_t reserved)
{
    uint8_t raddr_out[40];
    const uint8_t acc_id[20] =
      {
       0x2dU, 0xd8U, 0xaaU, 0xdbU, 0x4eU, 0x15U,               
       0xebU, 0xeaU,  0xeU, 0xfdU, 0x78U, 0xd1U, 0xb0U,
       0x35U, 0x91U,  0x4U, 0x7bU, 0xfaU, 0x1eU,  0xeU
      };

    int64_t bytes_written = 
      util_raddr(raddr_out, sizeof(raddr_out), acc_id, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: output of util_raddr can be precomputed [hooks-account-conv-pure]

    util_raddr(raddr_out, sizeof(raddr_out), acc_id, 20);
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: output of util_raddr can be precomputed [hooks-account-conv-pure]

    util_raddr(raddr_out, sizeof(raddr_out), 0, 0);

    if (util_raddr(raddr_out, sizeof(raddr_out), acc_id, sizeof(acc_id)) < 0) {
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: output of util_raddr can be precomputed [hooks-account-conv-pure]
      ++bytes_written;
    }

    return bytes_written;
}
