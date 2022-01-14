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
  Finder->addMatcher(callExpr(callee(functionDecl(hasName("etxn_reserve"))), anyOf(has(implicitCastExpr(has(integerLiteral().bind("signedConst")))), has(integerLiteral().bind("unsignedConst")))), this);
}

void ReserveLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const char *Literals[] = { "signedConst", "unsignedConst" };

  // if the call has both signed and unsigned arguments, it is wrong
  // (because etxn_reserve only takes a single parameter), but we
  // might still output limit diagnostics for both of them
  for (size_t i = 0; i < sizeof(Literals) / sizeof(Literals[0]); ++i) {
    const IntegerLiteral *ConstReserve = Result.Nodes.getNodeAs<IntegerLiteral>(Literals[i]);
    if (ConstReserve) {
      llvm::APInt Value = ConstReserve->getValue();
      uint64_t LimitedValue = Value.getLimitedValue();
      if (!LimitedValue) {
	diag(ConstReserve->getLocation(), "etxn_reserve need not be called to reserve 0 transactions");
      } else if (LimitedValue > MAX_EMIT) {
	diag(ConstReserve->getLocation(), "etxn_reserve may not reserve more than %0 transactions") << MAX_EMIT;
      }
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
