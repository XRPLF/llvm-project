// RUN: %check_clang_tidy %s hooks-raddr-conv-buf-len %t

#include <stdint.h>

extern int64_t util_accid(uint32_t write_ptr, uint32_t write_len,
			  uint32_t read_ptr,  uint32_t read_len);

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t carbon_accid[10];
    int64_t ret = util_accid(
	SBUF(carbon_accid),
	SBUF("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"));
// CHECK-MESSAGES: :[[@LINE-2]]:2: warning: output buffer of util_accid needs 20 bytes for account ID [hooks-raddr-conv-buf-len]

    uint8_t carbon_accid2[20];
    ret = util_accid(
	SBUF(carbon_accid2),
	SBUF("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"));
    
    return ret;
}
