//===--- EntryPointsCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EntryPointsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

const char *EntryPointsCheck::Names[] = { "cbak", "hook" };

void EntryPointsCheck::registerMatchers(MatchFinder *Finder) {
  for (size_t i = 0; i < sizeof(Names) / sizeof(Names[0]); ++i) {
    Finder->addMatcher(translationUnitDecl(unless(hasDescendant(functionDecl(isDefinition(), unless(isStaticStorageClass()), hasName(Names[i]))))).bind(getMatchName(Names[i])), this);
  }
}

void EntryPointsCheck::check(const MatchFinder::MatchResult &Result) {
  for (size_t i = 0; i < sizeof(Names) / sizeof(Names[0]); ++i) {
    const auto *Matched = Result.Nodes.getNodeAs<TranslationUnitDecl>(getMatchName(Names[i]));
    if (Matched) {
      diag("missing function '%0'", DiagnosticIDs::Error) << Names[i];
    }
  }
}

std::string EntryPointsCheck::getMatchName(const char *Name) const {
  std::string MatchName("~");
  MatchName += Name;
  return MatchName;
}

} // namespace hooks
} // namespace tidy
} // namespace clang
