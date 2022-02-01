//===--- FloatComparePureCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatComparePureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FloatComparePureCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("float_compare"))),
	     hasArgument(0, expr().bind("left")),
	     hasArgument(1, expr().bind("right")),
	     hasArgument(2, expr().bind("mode"))).bind("call");

  Finder->addMatcher(CallExpr, this);
}

void FloatComparePureCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Mode = Result.Nodes.getNodeAs<Expr>("mode");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> ModeValue = Mode->getIntegerConstantExpr(Context);
  if (ModeValue) {
    if ((*ModeValue < 1) || (*ModeValue > 6)) { // based on doc + API header
      diag(Mode->getBeginLoc(), "function float_compare has invalid mode");
      return;
    }

    const Expr *Left = Result.Nodes.getNodeAs<Expr>("left");
    const Expr *Right = Result.Nodes.getNodeAs<Expr>("right");
    Optional<llvm::APSInt> LeftValue = Left->getIntegerConstantExpr(Context);
    Optional<llvm::APSInt> RightValue = Right->getIntegerConstantExpr(Context);
    if (LeftValue && RightValue) {
      const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>("call");
      diag(Call->getBeginLoc(), "output of float_compare can be precomputed");
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
