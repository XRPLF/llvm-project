// RUN: %clang_cc1 -x objective-c %s -fsyntax-only -verify

#include <stdarg.h>

void f1(id arg) {
  NSLog(@"%@", arg); // expected-warning {{implicitly declaring library function 'NSLog' with type 'void (id, ...)'}}
}

void f2(id str, va_list args) {
  NSLogv(@"%@", args); // expected-warning {{implicitly declaring library function 'NSLogv' with type }}
}
