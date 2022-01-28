//===--- FieldDelBufLenCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FieldDelBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FieldDelBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("sto_erase"))),
	     hasArgument(1, expr().bind("outputSize")),
	     hasArgument(3, expr().bind("sourceSize")));

  Finder->addMatcher(CallExpr, this);
}

void FieldDelBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");
  const Expr *SourceSize = Result.Nodes.getNodeAs<Expr>("sourceSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(Context);  
  Optional<llvm::APSInt> SourceSizeValue = SourceSize->getIntegerConstantExpr(Context);  

  if (OutputSizeValue && SourceSizeValue && (*OutputSizeValue < *SourceSizeValue)) {
    diag(OutputSize->getBeginLoc(), "output buffer of sto_erase too small for input object");
  }

  if (SourceSizeValue && (*SourceSizeValue > MAX_SOURCE_SIZE)) {
    diag(SourceSize->getBeginLoc(), "input object of sto_erase can have at most %0 bytes") << MAX_SOURCE_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
