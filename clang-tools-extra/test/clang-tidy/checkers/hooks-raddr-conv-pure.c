// RUN: %check_clang_tidy %s hooks-raddr-conv-pure %t

#include <stdint.h>

extern int64_t util_accid(uint32_t write_ptr, uint32_t write_len,
			  uint32_t read_ptr,  uint32_t read_len);

extern int64_t rollback(uint32_t read_ptr,  uint32_t read_len,   int64_t error_code);

#define BLACKLIST_ACCOUNT "rno8D3qi5VrBvxTUE7nsem5kcJDnQUooQy"

#define SBUF(str) (uint32_t)(str), sizeof(str)

int64_t hook(int64_t reserved)
{
    uint8_t carbon_accid[20];
    int64_t ret = util_accid(
	(uint32_t)(carbon_accid),
	sizeof(carbon_accid),
	(uint32_t)("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"),
	sizeof("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"));
// CHECK-MESSAGES: :[[@LINE-2]]:13: warning: output of util_accid can be precomputed [hooks-raddr-conv-pure]

    ret = util_accid(
	(uint32_t)(carbon_accid),
	sizeof(carbon_accid),
	(uint32_t)("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"),
	sizeof("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"));
// CHECK-MESSAGES: :[[@LINE-2]]:13: warning: output of util_accid can be precomputed [hooks-raddr-conv-pure]

    uint8_t blacklist_accid[20];
    if (util_accid(SBUF(blacklist_accid), SBUF(BLACKLIST_ACCOUNT)) != 20)
	rollback(SBUF("Firewall: Could not decode blacklist account id."), 200);
// CHECK-MESSAGES: :[[@LINE-2]]:48: warning: output of util_accid can be precomputed [hooks-raddr-conv-pure]

    return ret;
}

// for some reason, AST doesn't match if there are too many calls to
// util_accid in a single function...
int64_t hook2(int64_t reserved)
{
    uint8_t carbon_accid[20];
    int64_t ret = util_accid(
	(uint32_t)(carbon_accid), 20,
	(uint32_t)("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"), 34);
// CHECK-MESSAGES: :[[@LINE-1]]:13: warning: output of util_accid can be precomputed [hooks-raddr-conv-pure]

    // no message unless the size argument is constant - changing
    // input size changes the input...
    ret = util_accid(
	(uint32_t)(carbon_accid),
	sizeof(carbon_accid),
	(uint32_t)("rfCarbonVNTuXckX6x2qTMFmFSnm6dEWGX"),
	reserved);

    return ret;
}
