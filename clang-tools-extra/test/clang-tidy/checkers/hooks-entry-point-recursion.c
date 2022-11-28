// RUN: %check_clang_tidy %s hooks-entry-point-recursion %t

int cbak() {
  return 0;
}

int hook() {
  return cbak();
// CHECK-MESSAGES: :[[@LINE-1]]:10: warning: hook functions may not call each other (nor themselves) [hooks-entry-point-recursion]
}
