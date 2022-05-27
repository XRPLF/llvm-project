//===--- TrivialCbakCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TrivialCbakCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void TrivialCbakCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(isDefinition(), hasName("cbak"), has(compoundStmt(statementCountIs(1), has(returnStmt())))).bind("trivial"), this);
}

void TrivialCbakCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<FunctionDecl>("trivial");
  if (Matched) {
    diag(Matched->getBeginLoc(), "'cbak' that just returns is not needed") << FixItHint::CreateRemoval(Matched->getSourceRange());
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
