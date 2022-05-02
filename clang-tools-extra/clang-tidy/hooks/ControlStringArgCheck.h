//===--- ControlStringArgCheck.h - clang-tidy -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_CONTROLSTRINGARGCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_CONTROLSTRINGARGCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Functions accept and rollback take an optional string buffer
/// stored outside the hook as its result message. Official hook
/// examples use this for debug messages, but for a release version
/// this wastes space (especially if the strings are constant) and
/// should be removed.
class ControlStringArgCheck : public ClangTidyCheck {
public:
  ControlStringArgCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context), Prep(NULL) {}
  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  Preprocessor *Prep;
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_CONTROLSTRINGARGCHECK_H
