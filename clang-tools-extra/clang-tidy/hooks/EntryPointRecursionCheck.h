//===--- EntryPointRecursionCheck.h - clang-tidy ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_ENTRYPOINTRECURSIONCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_ENTRYPOINTRECURSIONCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Calls to non-Hook API functions are disallowed in the Hooks
/// amendment. For library functions this can be enforced by not
/// declaring them; calls to functions defined by the hook are checked
/// here.
class EntryPointRecursionCheck : public ClangTidyCheck {
public:
  EntryPointRecursionCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_ENTRYPOINTRECURSIONCHECK_H
