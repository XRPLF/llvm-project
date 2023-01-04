//===--- GuardCallNonConstCheck.h - clang-tidy ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_GUARDCALLNONCONSTCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_GUARDCALLNONCONSTCHECK_H

#include "../ClangTidyCheck.h"

#include <unordered_set>

namespace clang {
namespace tidy {
namespace hooks {

/// Only compile-time constants can be used as an argument in loop GUARD call.
/// This checker prints warning if a non compile-time constant is used. 
/// It also checks whether a compile-time constant is used as a first argument
/// of _g() call and whether it is a unique value. If not - prints warning.

class GuardCallNonConstCheck : public ClangTidyCheck {
public:
  GuardCallNonConstCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  std::unordered_set<int> GuardIds;
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_GUARDCALLNONCONSTCHECK_H
