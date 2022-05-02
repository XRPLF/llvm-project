//===--- ControlStringArgCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ControlStringArgCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Preprocessor.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void ControlStringArgCheck::registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                                                Preprocessor *ModuleExpanderPP) {
  Prep = PP;
}

void ControlStringArgCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasAnyName("accept", "rollback")).bind("declaration")),
	     hasArgument(0, cStyleCastExpr(hasDescendant(stringLiteral())).bind("message")));

  Finder->addMatcher(CallExpr, this);  
}

void ControlStringArgCheck::check(const MatchFinder::MatchResult &Result) {
  // this check is for release mode only
  if (!(Prep && Prep->isMacroDefined("NDEBUG")))
    return;

  const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
  const Expr *Message = Result.Nodes.getNodeAs<Expr>("message");
  std::string FunctionName = Matched->getDeclName().getAsString();
  diag(Message->getBeginLoc(), "constant string argument of %0 wastes space") << FunctionName;
}

} // namespace hooks
} // namespace tidy
} // namespace clang
