//===--- SlotSubLimitCheck.h - clang-tidy -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_SLOTSUBLIMITCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_SLOTSUBLIMITCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Hook APIs slot_subarray and slot_subfield take parameters
/// specifying parent and child slot numbers. Values of these
/// parameters are limited, and the functions fail if the limit is
/// exceeded.
class SlotSubLimitCheck : public ClangTidyCheck {
public:
  SlotSubLimitCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  void checkSlot(const ast_matchers::MatchFinder::MatchResult &Result, const Expr *Slot, bool ZeroAllowed);

  const int64_t MAX_SLOT_NO = 255; // value copied from rippled source
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_SLOTSUBLIMITCHECK_H
