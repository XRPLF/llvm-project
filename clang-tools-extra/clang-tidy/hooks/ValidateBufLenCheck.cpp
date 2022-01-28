//===--- ValidateBufLenCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ValidateBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void ValidateBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("sto_validate"))),
	     hasArgument(1, expr().bind("bufferSize")));

  Finder->addMatcher(CallExpr, this);
}

void ValidateBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *BufferSize = Result.Nodes.getNodeAs<Expr>("bufferSize");

  assert(Result.Context);
  Optional<llvm::APSInt> BufferSizeValue = BufferSize->getIntegerConstantExpr(*(Result.Context));
  if (BufferSizeValue && !*BufferSizeValue) {
    diag(BufferSize->getBeginLoc(), "input buffer of sto_validate must not be empty");
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
