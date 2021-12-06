// RUN: %check_clang_tidy %s misc-hook-entry-points %t

// Declarations can be named without restrictions.
void accept();

void f() {
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: unknown exported function 'f' [misc-hook-entry-points]
}

// The parameter is missing but this check doesn't check that.
void cbak() {
}
