//===--- SlotLimitCheck.h - clang-tidy --------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_SLOTLIMITCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_SLOTLIMITCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Hook APIs slot, slot_clear, slot_count and slot_size take a
/// parameter specifying the accessed slot number. Value of this
/// parameter is limited, and the functions fail if the limit is
/// exceeded.
class SlotLimitCheck : public ClangTidyCheck {
public:
  SlotLimitCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  const int64_t MAX_SLOT_NO = 255; // value copied from rippled source
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_SLOTLIMITCHECK_H
