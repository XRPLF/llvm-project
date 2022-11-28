// RUN: %check_clang_tidy -expect-clang-tidy-error %s hooks-guard-called %t

// Declaration is not a problem.
long hook(long reserved);

long hook(long reserved) {
// CHECK-MESSAGES: :[[@LINE-1]]:6: error: 'hook' function does not call '_g' [hooks-guard-called]
  return 0l;
}

// The requirement to call _g is only on the hook function.
int cbak() {
  return 0;
}
