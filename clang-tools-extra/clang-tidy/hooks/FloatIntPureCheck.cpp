//===--- FloatIntPureCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatIntPureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FloatIntPureCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("float_int"))),
	     hasArgument(0, expr().bind("float")),
	     hasArgument(1, expr().bind("decimal")),
	     hasArgument(2, expr().bind("absolute"))).bind("call");

  Finder->addMatcher(CallExpr, this);
}

void FloatIntPureCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Decimal = Result.Nodes.getNodeAs<Expr>("decimal");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> DecimalValue = Decimal->getIntegerConstantExpr(Context);
  if (DecimalValue) {
    if ((*DecimalValue < 0) || (*DecimalValue > MAX_DECIMAL_PLACES)) {
      diag(Decimal->getBeginLoc(), "decimal places of float_int can be at most %0") << MAX_DECIMAL_PLACES;
      return;
    }

    const Expr *Float = Result.Nodes.getNodeAs<Expr>("float");
    const Expr *Absolute = Result.Nodes.getNodeAs<Expr>("absolute");
    Optional<llvm::APSInt> FloatValue = Float->getIntegerConstantExpr(Context);
    Optional<llvm::APSInt> AbsoluteValue = Absolute->getIntegerConstantExpr(Context);
    if (FloatValue && AbsoluteValue) {
      const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>("call");
      diag(Call->getBeginLoc(), "output of float_int can be precomputed");
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
