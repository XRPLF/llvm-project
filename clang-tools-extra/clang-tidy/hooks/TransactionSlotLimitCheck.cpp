//===--- TransactionSlotLimitCheck.cpp - clang-tidy -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TransactionSlotLimitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void TransactionSlotLimitCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("otxn_slot"))),
	     hasArgument(0, expr().bind("slot")));

  Finder->addMatcher(CallExpr, this);
}

void TransactionSlotLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Slot = Result.Nodes.getNodeAs<Expr>("slot");

  assert(Result.Context);
  Optional<llvm::APSInt> SlotValue = Slot->getIntegerConstantExpr(*(Result.Context));
  if (SlotValue) {
    llvm::APSInt LimitedValue = *SlotValue;
    // 0 means allocate new slot
    if ((LimitedValue < 0) || (LimitedValue > MAX_SLOT_NO)) {
      diag(Slot->getBeginLoc(), "function otxn_slot may not use more than %0 slots") << MAX_SLOT_NO;
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
