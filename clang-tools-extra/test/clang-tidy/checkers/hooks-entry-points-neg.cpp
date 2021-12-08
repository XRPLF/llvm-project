// RUN: %check_clang_tidy %s hooks-entry-points-neg %t

// Declarations can be named without restrictions.
void accept();

void f() {
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: unknown exported function 'f' [hooks-entry-points-neg]
}

// The parameter is missing but this check doesn't check that.
void cbak() {
}
