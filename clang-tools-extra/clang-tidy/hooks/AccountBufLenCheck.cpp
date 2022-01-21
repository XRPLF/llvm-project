//===--- AccountBufLenCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AccountBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void AccountBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("hook_account")).bind("declaration")),
	     hasArgument(1, expr().bind("outputSize")));

  Finder->addMatcher(CallExpr, this);
}

void AccountBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");
  const FunctionDecl *Declaration = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
  ASTContext &Context = Declaration->getASTContext();
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(Context);

  if (OutputSizeValue && (*OutputSizeValue < ACCOUNT_ID_SIZE)) {
    diag(OutputSize->getBeginLoc(), "output buffer of hook_account needs %0 bytes for account ID") << ACCOUNT_ID_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
