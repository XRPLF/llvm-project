// RUN: %check_clang_tidy -expect-clang-tidy-error %s hooks-entry-points %t
// CHECK-MESSAGES: error: missing function 'cbak' [hooks-entry-points]
// CHECK-MESSAGES: error: missing function 'hook' [hooks-entry-points]

void awesome_f2();
