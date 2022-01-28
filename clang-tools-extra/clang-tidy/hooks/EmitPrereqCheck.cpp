//===--- EmitPrereqCheck.cpp - clang-tidy ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EmitPrereqCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void EmitPrereqCheck::registerMatchers(MatchFinder *Finder) {
  // calls to emit & etxn_reserve should be in the same function
  // because hooks shouldn't have functions calling each other
  Finder->addMatcher(functionDecl(isDefinition(), hasDescendant(callExpr(callee(functionDecl(hasName("emit")))).bind("unsatisfied")), unless(hasDescendant(callExpr(callee(functionDecl(hasName("etxn_reserve"))))))), this);
}

void EmitPrereqCheck::check(const MatchFinder::MatchResult &Result) {
  const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>("unsatisfied");
  diag(Call->getBeginLoc(), "call of emit must be preceded by call of etxn_reserve");
}

} // namespace hooks
} // namespace tidy
} // namespace clang
