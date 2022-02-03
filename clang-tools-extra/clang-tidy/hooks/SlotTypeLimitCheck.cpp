//===--- SlotTypeLimitCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlotTypeLimitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void SlotTypeLimitCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("slot_type"))),
	     hasArgument(0, expr().bind("slot")),
	     hasArgument(1, expr().bind("flags")));

  Finder->addMatcher(CallExpr, this);
}

void SlotTypeLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Slot = Result.Nodes.getNodeAs<Expr>("slot");
  const Expr *Flags = Result.Nodes.getNodeAs<Expr>("flags");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> SlotValue = Slot->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> FlagsValue = Flags->getIntegerConstantExpr(Context);

  if (SlotValue) {
    llvm::APSInt LimitedValue = *SlotValue;
    // 0 not allowed
    if ((LimitedValue < 1) || (LimitedValue > MAX_SLOT_NO)) {
      diag(Slot->getBeginLoc(), "function slot_type may not access more than %0 slots") << MAX_SLOT_NO;
    }
  }

  if (FlagsValue) {
    llvm::APSInt LimitedValue = *FlagsValue;
    if ((LimitedValue < 0) || (LimitedValue > 1)) {
      diag(Slot->getBeginLoc(), "function slot_type accepts only 0 or 1 flags");
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
