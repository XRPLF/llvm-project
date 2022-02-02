//===--- SlotKeyletBufLenCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlotKeyletBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void SlotKeyletBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("slot_set"))),
	     hasArgument(1, expr().bind("inputSize")),
	     hasArgument(2, expr().bind("slot")));

  Finder->addMatcher(CallExpr, this);
}

void SlotKeyletBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *InputSize = Result.Nodes.getNodeAs<Expr>("inputSize");
  const Expr *Slot = Result.Nodes.getNodeAs<Expr>("slot");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> InputSizeValue = InputSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> SlotValue = Slot->getIntegerConstantExpr(Context);

  if (InputSizeValue) {
    llvm::APSInt ExpectedValue = *InputSizeValue;
    if ((ExpectedValue != SMALL_SIZE) && (ExpectedValue != LARGE_SIZE)) {
      diag(InputSize->getBeginLoc(), "input buffer of slot_set must have either %0 or %1 bytes") << SMALL_SIZE << LARGE_SIZE;
    }
  }

  if (SlotValue) {
    llvm::APSInt LimitedValue = *SlotValue;
    if ((LimitedValue < 0) || (LimitedValue > MAX_SLOT_NO)) {
      diag(Slot->getBeginLoc(), "function slot_set may not access more than %0 slots") << MAX_SLOT_NO;
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
