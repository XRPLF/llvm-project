//===--- ParamSetBufLenCheck.h - clang-tidy ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_PARAMSETBUFLENCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_PARAMSETBUFLENCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Function hook_param_set expects limited-length name, fixed-length
/// hash and limited-length value inputs.
class ParamSetBufLenCheck : public ClangTidyCheck {
public:
  ParamSetBufLenCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  const int64_t MIN_NAME_SIZE = 1; // documented
  const int64_t MAX_NAME_SIZE = 32; // documented
  const int64_t HASH_SIZE = 32; // documented
  const int64_t VALUE_SIZE = 128; // value copied from rippled source
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_PARAMSETBUFLENCHECK_H
