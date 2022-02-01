//===--- FloatArithPureCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATARITHPURECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATARITHPURECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Hooks can compute floating-point values in XFL format by calling
/// functions float_multiply, float_mulratio, float_negate, float_sum,
/// float_invert and float_divide and access their constituent parts by calling
/// float_exponent, float_mantissa and float_sign. If the inputs of
/// the computation never change, a more efficient way to do this is
/// to precompute it.
class FloatArithPureCheck : public ClangTidyCheck {
public:
  FloatArithPureCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  template<typename T>
  void registerCall(clang::ast_matchers::MatchFinder *Finder, T CallExpr);
  void checkCall(const ast_matchers::MatchFinder::MatchResult &Result, const CallExpr &Call, size_t N, size_t NonZeroParIdx=SIZE_MAX);
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_FLOATARITHPURECHECK_H
