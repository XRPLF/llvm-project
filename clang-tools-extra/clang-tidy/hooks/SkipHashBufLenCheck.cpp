//===--- SkipHashBufLenCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SkipHashBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void SkipHashBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("hook_skip"))),
	     hasArgument(1, expr().bind("inputSize")));

  Finder->addMatcher(CallExpr, this);
}

void SkipHashBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *InputSize = Result.Nodes.getNodeAs<Expr>("inputSize");

  assert(Result.Context);
  Optional<llvm::APSInt> InputSizeValue = InputSize->getIntegerConstantExpr(*(Result.Context));

  if (InputSizeValue && (*InputSizeValue != HASH_SIZE)) {
    diag(InputSize->getBeginLoc(), "hash buffer of hook_skip must have %0 bytes") << HASH_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
