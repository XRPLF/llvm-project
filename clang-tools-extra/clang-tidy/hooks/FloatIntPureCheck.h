//===--- FloatIntPureCheck.h - clang-tidy -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATINTPURECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATINTPURECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Hooks can convert floating-point values in XFL format to integers
/// by calling the float_int function. If the inputs of this function
/// never change, a more efficient way to do this is to precompute the
/// integer value.
class FloatIntPureCheck : public ClangTidyCheck {
public:
  FloatIntPureCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  const int64_t MAX_DECIMAL_PLACES = 15; // value copied from rippled source
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATINTPURECHECK_H
