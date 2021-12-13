//===--- EntryPointsCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EntryPointsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

const char *EntryPointsCheck::Names[] = { "cbak", "hook" };

namespace {

class DefinitionVisitor : public RecursiveASTVisitor<DefinitionVisitor> {
public:
  using Base = RecursiveASTVisitor<DefinitionVisitor>;

  const FunctionDecl *Anchor;
  const char *ExpectedName;
  bool Last;
  bool Matched;

  DefinitionVisitor(const FunctionDecl *Anchor, const char *ExpectedName): Anchor(Anchor), ExpectedName(ExpectedName), Last(false), Matched(false) {
    assert(Anchor);
    assert(ExpectedName);
  }

  bool TraverseFunctionDecl(FunctionDecl *D) {
    if (D->isThisDeclarationADefinition()) {
      Last = (D == Anchor);

      std::string Name = D->getDeclName().getAsString();
      if (Name == ExpectedName) {
	Matched = true;
      }
    }

    return Base::TraverseFunctionDecl(D);
  }
};

AST_MATCHER_P(FunctionDecl, isLastWithoutNameBefore, const char *, ExpectedName) {
  if (!Node.isThisDeclarationADefinition()) {
    return false;
  }

  DefinitionVisitor Visitor(&Node, ExpectedName);
  Visitor.TraverseAST(Finder->getASTContext());
  if (!Visitor.Last) {
    // Node not last function definition
    return false;
  }

  return !(Visitor.Matched);
}

}

void EntryPointsCheck::registerMatchers(MatchFinder *Finder) {
  for (size_t i = 0; i < sizeof(Names) / sizeof(Names[0]); ++i) {
    const char *Name = Names[i];
    Finder->addMatcher(functionDecl(isLastWithoutNameBefore(Name), has(compoundStmt().bind(getMatchName(Name)))), this);
  }
}

void EntryPointsCheck::check(const MatchFinder::MatchResult &Result) {
  for (size_t i = 0; i < sizeof(Names) / sizeof(Names[0]); ++i) {
    const char *Name = Names[i];
    const auto *Matched = Result.Nodes.getNodeAs<CompoundStmt>(getMatchName(Name));
    if (Matched) {
      diag(Matched->getEndLoc(), "missing function '%0'") << Name;
    }
  }
}

std::string EntryPointsCheck::getMatchName(const char *Name) const {
  assert(Name);
  std::string MatchName("~");
  MatchName += Name;
  return MatchName;
}

} // namespace hooks
} // namespace tidy
} // namespace clang
