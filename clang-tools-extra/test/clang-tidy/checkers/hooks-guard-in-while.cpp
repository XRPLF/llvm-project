// RUN: %check_clang_tidy %s hooks-guard-in-while %t

#include <stdint.h>

extern int32_t _g (uint32_t id, uint32_t maxiter);
extern int64_t trace_num (const char *read_ptr, uint32_t read_len, int64_t number);

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(int64_t reserved)
{
    int i = 1;
    while (i < 4)
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: while loop does not call '_g' [hooks-guard-in-while]
    {
	trace_num("one", 3, i);
	++i;
    }

    i = 0;
    while (GUARD(3), i < 3)
    {
	trace_num("two", 3, i);
	i += 1;
    }

    while (true)
      ;
// CHECK-MESSAGES: :[[@LINE-2]]:5: warning: while loop does not call '_g' [hooks-guard-in-while]


    return 0;
}
