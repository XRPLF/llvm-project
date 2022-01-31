//===--- FloatOnePureCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatOnePureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FloatOnePureCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(callExpr(callee(functionDecl(hasName("float_one")))).bind("call"), this);
}

void FloatOnePureCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Call = Result.Nodes.getNodeAs<CallExpr>("call");
  SourceRange CallRange(Call->getBeginLoc(), Call->getEndLoc());
  diag(Call->getBeginLoc(), "output of float_one is constant") << FixItHint::CreateReplacement(CallRange, "0x54838d7ea4c68000ll");
}

} // namespace hooks
} // namespace tidy
} // namespace clang
