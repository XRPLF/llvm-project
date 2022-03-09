//===--- GuardCalledCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GuardCalledCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void GuardCalledCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(isDefinition(), hasName("hook"), unless(hasDescendant(callExpr(callee(functionDecl(hasName("_g"))))))).bind("hook"), this);
}

void GuardCalledCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<FunctionDecl>("hook");
  diag(Matched->getLocation(), "'hook' function does not call '_g'", DiagnosticIDs::Error);
}

} // namespace hooks
} // namespace tidy
} // namespace clang
