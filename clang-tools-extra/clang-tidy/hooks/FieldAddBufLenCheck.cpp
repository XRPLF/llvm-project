//===--- FieldAddBufLenCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FieldAddBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FieldAddBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("sto_emplace"))),
	     hasArgument(1, expr().bind("outputSize")),
	     hasArgument(3, expr().bind("sourceSize")),
	     hasArgument(5, expr().bind("fieldSize")));

  Finder->addMatcher(CallExpr, this);
}

void FieldAddBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");
  const Expr *SourceSize = Result.Nodes.getNodeAs<Expr>("sourceSize");
  const Expr *FieldSize = Result.Nodes.getNodeAs<Expr>("fieldSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(Context);  
  Optional<llvm::APSInt> SourceSizeValue = SourceSize->getIntegerConstantExpr(Context);  
  Optional<llvm::APSInt> FieldSizeValue = FieldSize->getIntegerConstantExpr(Context);

  if (OutputSizeValue && SourceSizeValue && FieldSizeValue && (*OutputSizeValue < (*SourceSizeValue + *SourceSizeValue))) {
    diag(OutputSize->getBeginLoc(), "output buffer of sto_emplace too small for input object + input field");
  }

  if (SourceSizeValue && (*SourceSizeValue > MAX_SOURCE_SIZE)) {
    diag(SourceSize->getBeginLoc(), "input object of sto_emplace can have at most %0 bytes") << MAX_SOURCE_SIZE;
  }

  if (FieldSizeValue && (*FieldSizeValue > MAX_FIELD_SIZE)) {
    diag(FieldSize->getBeginLoc(), "input field of sto_emplace can have at most %0 bytes") << MAX_FIELD_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
