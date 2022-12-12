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
