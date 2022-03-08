// RUN: %clang_cc1 -x objective-c %s -fsyntax-only -verify
// rdar://8592641
Class f0(void) { return objc_getClass("a"); } // expected-warning {{implicitly declaring library function 'objc_getClass' with type 'id (const char *)'}}

// rdar://8735023
Class f1(void) { return objc_getMetaClass("a"); } // expected-warning {{implicitly declaring library function 'objc_getMetaClass' with type 'id (const char *)'}}

void f2(id val) { objc_enumerationMutation(val); } // expected-warning {{implicitly declaring library function 'objc_enumerationMutation' with type 'void (id)'}}

long double f3(id self, SEL op) { return objc_msgSend_fpret(self, op); } // expected-warning {{implicitly declaring library function 'objc_msgSend_fpret' with type 'long double (id, SEL, ...)'}}

id f4(struct objc_super *super, SEL op) { // expected-warning {{declaration of 'struct objc_super' will not be visible outside of this function}}
  return objc_msgSendSuper(super, op); // expected-warning {{implicitly declaring library function 'objc_msgSendSuper' with type 'id (struct objc_super *, SEL, ...)'}}
}

id f5(id val, id *dest) {
  return objc_assign_strongCast(val, dest); // expected-warning {{implicitly declaring library function 'objc_assign_strongCast' with type 'id (id, id *)'}}
}

int f6(Class exceptionClass, id exception) {
  return objc_exception_match(exceptionClass, exception); // expected-warning {{implicitly declaring library function 'objc_exception_match' with type 'int (id, id)'}}
}
