//===--- HashBufLenCheck.cpp - clang-tidy ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "HashBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void HashBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_sha512h"))),
	     hasArgument(1, expr().bind("outputSize")));

  Finder->addMatcher(CallExpr, this);
}

void HashBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");

  assert(Result.Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(*(Result.Context));

  if (OutputSizeValue && (*OutputSizeValue < HASH_SIZE)) {
    diag(OutputSize->getBeginLoc(), "output buffer of util_sha512h needs %0 bytes for the hash") << HASH_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
