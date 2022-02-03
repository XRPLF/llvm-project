//===--- SlotHashBufLenCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlotHashBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void SlotHashBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("slot_id"))),
	     hasArgument(1, expr().bind("outputSize")),
	     hasArgument(2, expr().bind("slot")));

  Finder->addMatcher(CallExpr, this);
}

void SlotHashBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");
  const Expr *Slot = Result.Nodes.getNodeAs<Expr>("slot");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> SlotValue = Slot->getIntegerConstantExpr(Context);

  if (OutputSizeValue && (*OutputSizeValue < HASH_SIZE)) {
    diag(OutputSize->getBeginLoc(), "output buffer of slot_id needs %0 bytes for the hash") << HASH_SIZE;
  }

  if (SlotValue) {
    llvm::APSInt LimitedValue = *SlotValue;
    // 0 not allowed
    if ((LimitedValue < 1) || (LimitedValue > MAX_SLOT_NO)) {
      diag(Slot->getBeginLoc(), "function slot_id may not access more than %0 slots") << MAX_SLOT_NO;
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
