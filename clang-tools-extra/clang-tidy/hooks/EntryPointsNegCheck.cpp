//===--- EntryPointsNegCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EntryPointsNegCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void EntryPointsNegCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(isDefinition()).bind("functionDefinition"), this);
}

void EntryPointsNegCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<FunctionDecl>("functionDefinition");
  std::string Name = Matched->getDeclName().getAsString();
  if ((Name != "cbak") && (Name != "hook")) {
    diag(Matched->getLocation(), "hook cannot define unknown function '%0'", DiagnosticIDs::Error) << Name;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
