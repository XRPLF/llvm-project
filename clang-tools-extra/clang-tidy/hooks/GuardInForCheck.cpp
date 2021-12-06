//===--- GuardInForCheck.cpp - clang-tidy ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GuardInForCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void GuardInForCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(forStmt(unless(has(binaryOperator(has(callExpr(callee(functionDecl(hasName("_g"))))))))).bind("unguarded"), this);
}

void GuardInForCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<ForStmt>("unguarded");
  diag(Matched->getBeginLoc(), "for loop does not call '_g'");
}

} // namespace hooks
} // namespace tidy
} // namespace clang
