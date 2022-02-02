//===--- SlotSubLimitCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlotSubLimitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void SlotSubLimitCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasAnyName("slot_subarray", "slot_subfield")).bind("declaration")),
	     hasArgument(0, expr().bind("parent")),
	     hasArgument(2, expr().bind("child")));

  Finder->addMatcher(CallExpr, this);
}

void SlotSubLimitCheck::check(const MatchFinder::MatchResult &Result) {
  checkSlot(Result, Result.Nodes.getNodeAs<Expr>("parent"), false);
  checkSlot(Result, Result.Nodes.getNodeAs<Expr>("child"), true);
}

void SlotSubLimitCheck::checkSlot(const MatchFinder::MatchResult &Result, const Expr *Slot, bool ZeroAllowed) {
  assert(Slot);
  assert(Result.Context);
  Optional<llvm::APSInt> Value = Slot->getIntegerConstantExpr(*(Result.Context));
  if (Value) {
    llvm::APSInt LimitedValue = *Value;
    if ((LimitedValue < 0) || (!LimitedValue && !ZeroAllowed) || (LimitedValue > MAX_SLOT_NO)) {
      const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
      std::string FunctionName = Matched->getDeclName().getAsString();
      diag(Slot->getBeginLoc(), "function %0 may not access more than %1 slots") <<
	FunctionName << MAX_SLOT_NO;
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
