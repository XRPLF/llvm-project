//===--- SlotLimitCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SlotLimitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void SlotLimitCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("slot")).bind("declaration")),
	     hasArgument(2, expr().bind("argument")));

  const auto CallExpr2 =
    callExpr(callee(functionDecl(hasAnyName("slot_clear", "slot_count", "slot_size")).bind("declaration")),
	     hasArgument(0, expr().bind("argument")));

  Finder->addMatcher(CallExpr, this);
  Finder->addMatcher(CallExpr2, this);
}

void SlotLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Argument = Result.Nodes.getNodeAs<Expr>("argument");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> ArgumentValue = Argument->getIntegerConstantExpr(Context);
  if (ArgumentValue) {
    llvm::APSInt LimitedValue = *ArgumentValue;
    // none of the matched functions allows 0
    if ((LimitedValue < 1) || (LimitedValue > MAX_SLOT_NO)) {
      const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
      std::string FunctionName = Matched->getDeclName().getAsString();
      diag(Argument->getBeginLoc(), "function %0 may not access more than %1 slots") <<
	FunctionName << MAX_SLOT_NO;
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
