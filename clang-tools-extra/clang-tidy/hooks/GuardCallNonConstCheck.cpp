//===--- GuardCallNonConstCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GuardCallNonConstCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

auto unsupportedGuardCall() {
  auto guardArg = anyOf(
        ignoringParenImpCasts(declRefExpr(to(varDecl(hasType(isConstQualified()))))), 
        ignoringParenCasts(integerLiteral())
      );

  return callExpr(callee(functionDecl(
      hasName("_g"))),
      unless(hasArgument(1, anyOf(
        binaryOperator(hasOperatorName("+"), hasLHS(guardArg)),
        guardArg
      )
  )));
}

void GuardCallNonConstCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(stmt(unsupportedGuardCall()).bind("unsupportedGuardedCall"), this);
}

void GuardCallNonConstCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *UnsupportedGuardCall = Result.Nodes.getNodeAs<Stmt>("unsupportedGuardedCall");
  if (UnsupportedGuardCall) {
    auto StartLoc = UnsupportedGuardCall->getBeginLoc();
    auto EndLoc = UnsupportedGuardCall->getEndLoc();//StartLoc.getLocWithOffset(sizeof("GUARD(x)") - 1);
    diag(UnsupportedGuardCall->getBeginLoc(), "'GUARD' calls can only have const integers or literals as an argument") <<
      SourceRange(StartLoc, EndLoc);
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
