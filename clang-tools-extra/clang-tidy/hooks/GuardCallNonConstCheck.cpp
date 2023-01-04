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
  return callExpr(callee(functionDecl(
      hasName("_g"))),
      hasArgument(0, expr().bind("guardIdExpr")),
      hasArgument(1, expr().bind("guardLimitExpr"))
  );
}

Optional<int> getConstValue(const Expr *literal, const VarDecl *constDecl, ASTContext &context) {
  if (literal) {
    if (auto val = literal->getIntegerConstantExpr(context)) {
      return val->getExtValue();
    }
  }
  if (constDecl) {
    if (auto val = constDecl->evaluateValue()) {
      return val->getInt().getExtValue();
    }
  }
  return {};
}

void GuardCallNonConstCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(stmt(unsupportedGuardCall()).bind("unsupportedGuardedCall"), this);
}

void GuardCallNonConstCheck::check(const MatchFinder::MatchResult &Result) {
  ASTContext &Context = *(Result.Context);
  const auto *UnsupportedGuardCall = Result.Nodes.getNodeAs<Stmt>("unsupportedGuardedCall");
  if (UnsupportedGuardCall) {
    const Expr *GuardLimitExpr = Result.Nodes.getNodeAs<Expr>("guardLimitExpr");
    const Expr *GuardIdExpr = Result.Nodes.getNodeAs<Expr>("guardIdExpr");

    bool NonConstExpr = !GuardLimitExpr || !GuardLimitExpr->isEvaluatable(Context);
    if (NonConstExpr) {
      auto StartLoc = UnsupportedGuardCall->getBeginLoc();
      auto EndLoc = UnsupportedGuardCall->getEndLoc();
      diag(UnsupportedGuardCall->getBeginLoc(), "'GUARD' calls can only have compile-time constant as an argument") <<
        SourceRange(StartLoc, EndLoc);
    }
    if (GuardIdExpr && GuardIdExpr->isEvaluatable(Context)) {
      Expr::EvalResult Result;
      GuardIdExpr->EvaluateAsRValue(Result, Context, true);

      int value = Result.Val.getInt().getExtValue();

      if (GuardIds.find(value) != GuardIds.end()) {
        auto StartLoc = UnsupportedGuardCall->getBeginLoc();
        auto EndLoc = UnsupportedGuardCall->getEndLoc();
        diag(UnsupportedGuardCall->getBeginLoc(), "'GUARD' call guard ID argument must be unique") <<
          SourceRange(StartLoc, EndLoc);
      }
      GuardIds.insert(value);
    }
    else {
      auto StartLoc = UnsupportedGuardCall->getBeginLoc();
      auto EndLoc = UnsupportedGuardCall->getEndLoc();
      diag(UnsupportedGuardCall->getBeginLoc(), "'GUARD' call guard ID argument must be compile-time constant") <<
        SourceRange(StartLoc, EndLoc);
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
