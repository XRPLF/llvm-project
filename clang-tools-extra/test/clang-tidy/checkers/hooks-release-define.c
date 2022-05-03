// RUN: %check_clang_tidy %s hooks-release-define %t

// tests apparently define NDEBUG by default
#undef NDEBUG

#include <stdint.h>

extern int64_t trace_num(uint32_t read_ptr, uint32_t read_len, int64_t number);

#ifdef NDEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif

#define TRACEVAR(v) if (DEBUG) trace_num((uint32_t)(#v), (uint32_t)(sizeof(#v) - 1), (int64_t)v);

#define NDEBUG

int64_t hook(uint32_t reserved)
{
    TRACEVAR(reserved);
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: NDEBUG ignored when defined after including hook-specific headers [hooks-release-define]

#undef DEBUG
#define DEBUG 0
    TRACEVAR(reserved);

    return 0;
}
