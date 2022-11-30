// RUN: %check_clang_tidy %s hooks-guard-in-for %t

#include <stdint.h>

extern int32_t _g (uint32_t id, uint32_t maxiter);
extern int64_t trace_num (const char *read_ptr, uint32_t read_len, int64_t number);

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(int64_t reserved)
{
    // syntax for testing warning with a proposed fix is unclear...
#if 0
    for (int i = 1; i < 3; ++i)
    {
	trace_num("one", 3, i);
    }

    for (int i = 0; i < 2; ++i)
    {
	trace_num("two", 3, i);
    }
#endif

    for (int i = 0; GUARD(3), i < 3; ++i)
    {
	trace_num("three", 5, i);
    }

    // ...but infinite looping certainly isn't allowed
    for (;;);
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: for loop does not call 'GUARD' [hooks-guard-in-for]

    for (int i = 0; 2 > i; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
    {
	trace_num("four", 4, i);
    }

    int i;
    for (i = 1; i < 3; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:17: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
    {
	trace_num("five", 4, i);
    }

    for (int i = 0; i < 1; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
    {
        trace_num("six", 3, i);
        for (int j = 0; j < 2; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
        {
            trace_num("seven", 5, j);
            for (int k = 0; GUARD(4), k < 3; ++k)
            {
                for (int w = 0; w < 4; ++w)
// CHECK-MESSAGES: :[[@LINE-1]]:33: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
                {
                    trace_num("eight", 5, w);
                }
            }
        }
    }

    return 0;
}
