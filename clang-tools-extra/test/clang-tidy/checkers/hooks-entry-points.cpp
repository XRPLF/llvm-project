// RUN: %check_clang_tidy %s hooks-entry-points %t

void f() {
}

// The parameter is missing but this check doesn't check that.
void cbak() {
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: missing function 'hook' [hooks-entry-points]
}
