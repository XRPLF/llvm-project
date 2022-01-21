//===--- RaddrConvBufLenCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "RaddrConvBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void RaddrConvBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_accid")).bind("declaration")),
	     hasArgument(1, expr().bind("outputSize")),
	     hasArgument(3, expr().bind("inputSize")));

  Finder->addMatcher(CallExpr, this);
}

void RaddrConvBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *InputSize = Result.Nodes.getNodeAs<Expr>("inputSize");
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");
  const FunctionDecl *Declaration = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
  ASTContext &Context = Declaration->getASTContext();
  Optional<llvm::APSInt> InputSizeValue = InputSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(Context);

  if (InputSizeValue && (*InputSizeValue > MAX_INPUT_SIZE)) {
    diag(InputSize->getBeginLoc(), "input raddr of util_accid may have at most %0 bytes") << MAX_INPUT_SIZE;
  }

  if (OutputSizeValue && (*OutputSizeValue < OUTPUT_SIZE)) {
    diag(OutputSize->getBeginLoc(), "output buffer of util_accid needs %0 bytes for account ID") << OUTPUT_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
