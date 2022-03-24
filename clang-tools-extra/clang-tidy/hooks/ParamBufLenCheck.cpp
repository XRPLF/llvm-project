//===--- ParamBufLenCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ParamBufLenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void ParamBufLenCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("hook_param"))),
             hasArgument(1, expr().bind("valueSize")),
	     hasArgument(3, expr().bind("nameSize")));

  Finder->addMatcher(CallExpr, this);
}

void ParamBufLenCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *ValueSize = Result.Nodes.getNodeAs<Expr>("valueSize");
  const Expr *NameSize = Result.Nodes.getNodeAs<Expr>("nameSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> ValueSizeValue = ValueSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> NameSizeValue = NameSize->getIntegerConstantExpr(Context);

  if (ValueSizeValue && (*ValueSizeValue < VALUE_SIZE)) {
    diag(ValueSize->getBeginLoc(), "value buffer of function hook_param must have at least %0 bytes") << VALUE_SIZE;
  }

  if (NameSizeValue && ((*NameSizeValue < MIN_NAME_SIZE) || (*NameSizeValue > MAX_NAME_SIZE))) {
    diag(NameSize->getBeginLoc(), "name buffer of function hook_param must have between %0 and %1 bytes") << MIN_NAME_SIZE << MAX_NAME_SIZE;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
