//===--- ReserveLimitCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ReserveLimitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void ReserveLimitCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(callExpr(callee(functionDecl(hasName("etxn_reserve"))), has(implicitCastExpr(has(integerLiteral().bind("constReserve"))))).bind("call"), this);
}

void ReserveLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const IntegerLiteral *ConstReserve = Result.Nodes.getNodeAs<IntegerLiteral>("constReserve");
  llvm::APInt Value = ConstReserve->getValue();
  uint64_t LimitedValue = Value.getLimitedValue();
  if (!LimitedValue) {
    const auto *Matched = Result.Nodes.getNodeAs<CallExpr>("call");
    diag(Matched->getExprLoc(), "etxn_reserve need not be called to reserve 0 transactions");
  } else if (LimitedValue > MAX_EMIT) {
    const auto *Matched = Result.Nodes.getNodeAs<CallExpr>("call");
    diag(Matched->getExprLoc(), "etxn_reserve may not reserve more than %0 transactions") << MAX_EMIT;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
