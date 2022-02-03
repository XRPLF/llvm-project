//===--- StateBufLenCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StateBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void StateBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasAnyName("state", "state_set")).bind("declaration")),
	     hasArgument(3, expr().bind("keySize")));

  Finder->addMatcher(CallExpr, this);
}

void StateBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *KeySize = Result.Nodes.getNodeAs<Expr>("keySize");

  assert(Result.Context);
  Optional<llvm::APSInt> KeySizeValue = KeySize->getIntegerConstantExpr(*(Result.Context));

  if (KeySizeValue && (*KeySizeValue != KEY_SIZE)) {
    const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
    std::string FunctionName = Matched->getDeclName().getAsString();
    diag(KeySize->getBeginLoc(), "key buffer of function %0 should have %1 bytes") <<
      FunctionName << KEY_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
