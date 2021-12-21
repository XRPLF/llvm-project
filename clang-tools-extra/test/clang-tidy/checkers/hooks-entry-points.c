// RUN: %check_clang_tidy -expect-clang-tidy-error %s hooks-entry-points %t

void f() {
}

// The parameter is missing but this check doesn't check that.
void cbak() {
}
// CHECK-MESSAGES: :[[@LINE+1]]:1: error: missing function 'hook' [hooks-entry-points]
