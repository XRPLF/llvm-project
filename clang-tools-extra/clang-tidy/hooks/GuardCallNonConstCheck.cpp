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

namespace {

auto guardCall() {
  return callExpr(callee(functionDecl(
      hasName("_g"))),
      hasArgument(0, expr().bind("guardIdExpr")),
      hasArgument(1, expr().bind("guardLimitExpr"))
  );
}

}

void GuardCallNonConstCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(guardCall().bind("guardedCall"), this);
}

void GuardCallNonConstCheck::check(const MatchFinder::MatchResult &Result) {
  ASTContext &Context = *(Result.Context);
  const auto *GuardCall = Result.Nodes.getNodeAs<Stmt>("guardedCall");
  if (GuardCall) {
    const Expr *GuardLimitExpr = Result.Nodes.getNodeAs<Expr>("guardLimitExpr");
    const Expr *GuardIdExpr = Result.Nodes.getNodeAs<Expr>("guardIdExpr");

    bool NonConstExpr = !GuardLimitExpr || !GuardLimitExpr->isEvaluatable(Context);
    if (NonConstExpr) {
      auto StartLoc = GuardLimitExpr ? GuardLimitExpr->getBeginLoc() : GuardCall->getBeginLoc();
      auto EndLoc = GuardLimitExpr ? GuardLimitExpr->getEndLoc() : GuardCall->getEndLoc();

      diag(StartLoc, "'GUARD' calls can only have compile-time constant as an argument") <<
        SourceRange(StartLoc, EndLoc);
    }
    if (GuardIdExpr && GuardIdExpr->isEvaluatable(Context)) {
      Expr::EvalResult Result;
      GuardIdExpr->EvaluateAsRValue(Result, Context, true);

      int value = Result.Val.getInt().getExtValue();

      if (GuardIds.find(value) != GuardIds.end()) {
        auto StartLoc = GuardIdExpr->getBeginLoc();
        auto EndLoc = GuardIdExpr->getEndLoc();
        diag(StartLoc, "'GUARD' call guard ID argument must be unique") <<
          SourceRange(StartLoc, EndLoc);
      }
      GuardIds.insert(value);
    }
    else {
      auto StartLoc = GuardIdExpr ? GuardIdExpr->getBeginLoc() : GuardCall->getBeginLoc();
      auto EndLoc = GuardIdExpr ? GuardIdExpr->getEndLoc() : GuardCall->getEndLoc();
      diag(StartLoc, "'GUARD' call guard ID argument must be compile-time constant") <<
        SourceRange(StartLoc, EndLoc);
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
