//===--- ArrayBufLenCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ArrayBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void ArrayBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("sto_subarray"))),
	     hasArgument(1, expr().bind("bufferSize")),
	     hasArgument(2, expr().bind("index")));

  Finder->addMatcher(CallExpr, this);
}

void ArrayBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *BufferSize = Result.Nodes.getNodeAs<Expr>("bufferSize");
  const Expr *Index = Result.Nodes.getNodeAs<Expr>("index");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> BufferSizeValue = BufferSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> IndexValue = Index->getIntegerConstantExpr(Context);

  if (BufferSizeValue && !*BufferSizeValue) {
    diag(BufferSize->getBeginLoc(), "input buffer of sto_subarray must not be empty");
  }

  if (IndexValue && (*IndexValue > MAX_INDEX)) {
    diag(Index->getBeginLoc(), "array index of sto_subarray can be at most %0") << MAX_INDEX;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
