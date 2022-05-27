// RUN: %check_clang_tidy %s hooks-trivial-cbak %t

#include <stdint.h>

// The parameter is missing but this check doesn't check that.
int64_t cbak() {
// CHECK-MESSAGES: :[[@LINE-1]]:1: warning: 'cbak' that just returns is not needed [hooks-trivial-cbak]
    return 0;
}
