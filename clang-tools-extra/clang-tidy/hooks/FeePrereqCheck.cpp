//===--- FeePrereqCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FeePrereqCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FeePrereqCheck::registerMatchers(MatchFinder *Finder) {
  // calls to etxn_fee_base & etxn_reserve should be in the same
  // function because hooks shouldn't have functions calling each
  // other
  Finder->addMatcher(functionDecl(isDefinition(), hasDescendant(callExpr(callee(functionDecl(hasName("etxn_fee_base")))).bind("unsatisfied")), unless(hasDescendant(callExpr(callee(functionDecl(hasName("etxn_reserve"))))))), this);
}

void FeePrereqCheck::check(const MatchFinder::MatchResult &Result) {
  const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>("unsatisfied");
  diag(Call->getBeginLoc(), "call of etxn_fee_base must be preceded by call of etxn_reserve");
}

} // namespace hooks
} // namespace tidy
} // namespace clang
