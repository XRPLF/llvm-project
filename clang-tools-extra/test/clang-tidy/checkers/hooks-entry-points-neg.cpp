// RUN: %check_clang_tidy -expect-clang-tidy-error %s hooks-entry-points-neg %t

// Declarations can be named without restrictions.
void accept();

static void f() {
// CHECK-MESSAGES: :[[@LINE-1]]:13: error: hook cannot define unknown function 'f' [hooks-entry-points-neg]
}

// The parameter is missing but this check doesn't check that.
void cbak() {
}
