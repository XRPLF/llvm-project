//===--- RaddrConvPureCheck.h - clang-tidy ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_RADDRCONVPURECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_RADDRCONVPURECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Hooks identify accounts by the 20 byte account ID, which can be
/// converted from an raddr using the util_accid function. If the
/// raddr never changes, a more efficient way to do this is precompute
/// the account ID from the raddr.
class RaddrConvPureCheck : public ClangTidyCheck {
public:
  RaddrConvPureCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_RADDRCONVPURECHECK_H
