//===--- VerifyBufLenCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "VerifyBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void VerifyBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_verify"))),
	     hasArgument(3, expr().bind("signatureSize")),
	     hasArgument(5, expr().bind("keySize")));

  Finder->addMatcher(CallExpr, this);
}

void VerifyBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *KeySize = Result.Nodes.getNodeAs<Expr>("keySize");
  const Expr *SigSize = Result.Nodes.getNodeAs<Expr>("signatureSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> KeySizeValue = KeySize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> SigSizeValue = SigSize->getIntegerConstantExpr(Context);

  if (KeySizeValue && (*KeySizeValue != KEY_SIZE)) {
    diag(KeySize->getBeginLoc(), "key of util_verify must have %0 bytes") << KEY_SIZE;
  }

  if (SigSizeValue && ((*SigSizeValue < MIN_SIG_SIZE) || (*SigSizeValue > MAX_SIG_SIZE))) {
    diag(SigSize->getBeginLoc(), "signature of util_verify must have between %0 and %1 bytes") << MIN_SIG_SIZE << MAX_SIG_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
