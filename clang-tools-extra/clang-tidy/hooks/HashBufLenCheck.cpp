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
    callExpr(callee(functionDecl(hasAnyName("hook_hash", "ledger_last_hash", "nonce", "util_sha512h")).bind("declaration")),
	     hasArgument(1, expr().bind("outputSize")));

  Finder->addMatcher(CallExpr, this);
}

void HashBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");

  assert(Result.Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(*(Result.Context));

  if (OutputSizeValue && (*OutputSizeValue < HASH_SIZE)) {
    const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
    std::string FunctionName = Matched->getDeclName().getAsString();
    diag(OutputSize->getBeginLoc(), "output buffer of %0 needs %1 bytes for the hash") <<
      FunctionName << HASH_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
