//===--- GuardInForCheck.h - clang-tidy -------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_GUARDINFORCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_GUARDINFORCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Consider the following for-loop in C:
///
/// #define GUARD(maxiter) _g(__LINE__, (maxiter)+1)
/// for (int i = 0; GUARD(3), i < 3; ++i)
///
/// To satisfy the guard rule when using a for-loop in C guard should be 
/// place either in the condition part of the loop, or as a first call in loop body, e.g.
///
/// for(int i = 0; i < 3; ++i) {
///   GUARD(3);
/// }
/// 
/// In case of nested loops, the guard limit value should be 
/// multiplied by a number of iterations in each loop, e.g.
///
/// for(int i = 0; GUARD(3), i < 3; ++i) {
///   for (int j = 0; GUARD(17), j < 5; ++j)
/// }
/// 
/// (most descendant loop iterations + 1) * (each parent loops iterations) - 1
///
/// This checker will also calculate the GUARD limit and show a hint with proposed value 
/// next to a warning. Calculation is based on for loop parameters: init value, condition 
/// limit and loop increment. There are few requirements for the calculation to be 
/// performed correctly:
/// - loop index must be initialized inside the init part of the loop (however the 
///   variable can be declared outside of the for loop)
/// - there must be a proper condition limit set in the condition part of the loop, or a 
///   GUARD call must placed there (in which case the GUARD limit calculation of nested 
///   loops will be based on it)
/// - there must be a proper increment defined (e.g. i++, i += 2, i = i * 2)
/// - loop condition can contain multiple logical expressions, like "i < 2 && k > 3" or "k < 3, i < 2"
///   however in case of the "," operator it is not verified whether the part with loop 
///   index is on the right hand side of the operator
/// - there can be more than one variable initialized in the init part of the for loop
/// - in case of nested for loops GUARD limit is calculated only from parent for loops 
///   parameters (or existing GUARD calls), any other statements (if, while, etc.) are ignored
/// - if GUARD call is placed in a parent for loop it's value is used to calculate 
///   the GUARD limit for nested loops - it will not be check whether the GUARD value 
///   placed by the programmer is correct
/// - for loop init, condition limit and increment values must be either integer literals or
///   const int variables, so that their values can be evaluated during compile time - they 
///   cannot be expressions like i = 5 + 4, etc.
class GuardInForCheck : public ClangTidyCheck {
public:
  GuardInForCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_GUARDINFORCHECK_H
