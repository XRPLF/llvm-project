//===--- HookEntryPointsCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "HookEntryPointsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void HookEntryPointsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(isDefinition(), unless(isStaticStorageClass())).bind("functionDefinition"), this);
}

void HookEntryPointsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<FunctionDecl>("functionDefinition");
  std::string Name = Matched->getDeclName().getAsString();
  if ((Name != "cbak") && (Name != "hook")) {
    diag(Matched->getLocation(), "unknown exported function '%0'") << Name;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
