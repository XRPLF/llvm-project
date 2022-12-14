// RUN: %check_clang_tidy %s hooks-guard-in-for %t

#include "stdint.h"

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
// CHECK-MESSAGES-NOT: GUARD

    for (int i = 0; 2 > i; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(2)
    {
	trace_num("four", 4, i);
    }

    int i;
    for (i = 1; i < 3; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:17: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(2)
    {
	trace_num("five", 4, i);
    }

    for (int i = 0; i < 1; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(1)
    {
        for (int j = 0; j < 2; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(2)
        {
            for (int k = 0; GUARD(7), k < 3; ++k)
            {
                for (int w = 0; w < 4; ++w)
// CHECK-MESSAGES: :[[@LINE-1]]:33: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(29)
                {
                    trace_num("six", 3, w);
                }
            }
        }
    }

    for (int i = 0; i < 5; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(5)
    {
        while (1)
        {
            for (int j = 0; j < 3; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:13: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-MESSAGES-NOT: GUARD
            {
                trace_num("seven", 5, j);
            }
        }
    }

    for (int i = 0; i < 5 + reserved; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:5: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-MESSAGES-NOT: GUARD
    {
        for (int j = 0; j < 3; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-MESSAGES-NOT: GUARD
            {
                trace_num("eight", 5, j);
            }
    }

    for (int i = 0; GUARD(5), i < 5 + reserved; ++i)
    {
        for (int j = 0; j < 3; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(19)
        {
            trace_num("nine", 4, j);
        }
    }

    for (int i = 0; i <= 5; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(6)
    {
        trace_num("ten", 3, i);
    }

    for (int i = 0; 5 != i; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(5)
    {
        for (int j = 0; j != 5; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(29)
        {
            trace_num("eleven", 6, j);
        }
    }

    for (int j = 5; j > 0; --j)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(5)
    {
        trace_num("twelve", 6, j);
    }

    for (int j = 5; 0 <= j; --j)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(6)
    {
        trace_num("thirteen", 8, j);
    }

    for (int i = 0; i < 5; ++i)
    {
        GUARD(5);
        for (int j = 0; j < 3; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(19)
        {
            trace_num("fourteen", 8, j);
        }
    }

    for (int i = 0; i < 5; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(5)
    {
        for (int j = 0; GUARD(19), j < 3; ++j)
        {
            trace_num("fifteen", 7, j);
        }
    }

    for (int i = 0; i < 5; ++i) 
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(5)
    {
        for (int j = 0; j < 3; ++j)
        {
            GUARD(19);
            trace_num("sixteen", 7, j);
        }
    }

    for (int i = 0; i < 10; i += 2)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(5)
    {
        trace_num("sixteen", 7, i);
    }

    for (int i = 1; 10 > i; i *= 2)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(4)
    {
        trace_num("seventeen", 9, i);
    }

    for (int i = 10; i >= 0; i = i - 2)
// CHECK-MESSAGES: :[[@LINE-1]]:22: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(6)
    {
        trace_num("eighteen", 8, i);
    }

    for (int i = 10; i > 0; i = i / 2)
// CHECK-MESSAGES: :[[@LINE-1]]:22: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(4)
    {
        trace_num("nineteen", 8, i);
    }

    int k = 0;
    for (int i = 0; k < 2, i < 4; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:28: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(4)
    {
        trace_num("twenty", 6, i);
    }
    
    int j = 0;
    for (int i = 0; k < 2, i < 4 && j < 3; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:28: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(4)
    {
        trace_num("twenty one", 10, i);
    }

    for (int i = 0; GUARD(reserved), i < 10; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: 'GUARD' calls can only have const integers or literals as an argument [hooks-guard-in-for]
    {
        trace_num("twenty two", 10, i);
    }

    const int x = 5;
    for (int i = 0; GUARD(x), i < 10; ++i)
    {
        trace_num("twenty three", 12, i);
    }

    for (int i = 0; GUARD(reserved), i < 10; ++i)
// CHECK-MESSAGES: :[[@LINE-1]]:21: warning: 'GUARD' calls can only have const integers or literals as an argument [hooks-guard-in-for]
    {
        for (int j = 0; j < 2; ++j)
// CHECK-MESSAGES: :[[@LINE-1]]:25: warning: for loop does not call 'GUARD' [hooks-guard-in-for]
// CHECK-FIXES: GUARD(29)
        {
            trace_num("twenty four", 11, j);
        }
    }

    return 0;
}
