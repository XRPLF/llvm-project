//===--- FloatManipPureCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATMANIPPURECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATMANIPPURECHECK_H

#include "../ClangTidyCheck.h"
#include <stdint.h>

namespace clang {
namespace tidy {
namespace hooks {

/// Hooks can directly manipulate floating-point values in XFL format
/// by calling functions float_exponent_set, float_mantissa_set and
/// float_sign_set. If the inputs of the update never change, a more
/// efficient way to do this is to precompute it.
class FloatManipPureCheck : public ClangTidyCheck {
public:
  FloatManipPureCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  template<typename T>
  void registerCall(clang::ast_matchers::MatchFinder *Finder, T CallExpr);
  void checkCall(const ast_matchers::MatchFinder::MatchResult &Result, const CallExpr &Call, int64_t Min, int64_t Max);

  static const char *BoundNames[];
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATMANIPPURECHECK_H
