//===--- DetailBufLenCheck.h - clang-tidy -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_DETAILBUFLENCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_DETAILBUFLENCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

/// Function etxn_details has fixed-size sfEmitDetails output.
class DetailBufLenCheck : public ClangTidyCheck {
public:
  DetailBufLenCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  const int64_t ETXN_DETAILS_SIZE = 105; // documented
};

} // namespace hooks
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_HOOKS_DETAILBUFLENCHECK_H
