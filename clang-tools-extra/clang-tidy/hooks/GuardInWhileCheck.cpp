//===--- GuardInWhileCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GuardInWhileCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void GuardInWhileCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(whileStmt(unless(has(binaryOperator(has(callExpr(callee(functionDecl(hasName("_g"))))))))).bind("unguarded"), this);
}

void GuardInWhileCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<WhileStmt>("unguarded");
  SourceLocation Loc = Matched->getBeginLoc();
  diag(Loc, "while loop does not call '_g'") <<
      SourceRange(Loc, Matched->getRParenLoc());
}

} // namespace hooks
} // namespace tidy
} // namespace clang
