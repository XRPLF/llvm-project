//===--- FuncAddrTakenCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FuncAddrTakenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void FuncAddrTakenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(conditionalOperator(has(implicitCastExpr(has(declRefExpr(hasType(functionType())))))).bind("ternary"), this);
}

void FuncAddrTakenCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Ternary = Result.Nodes.getNodeAs<ConditionalOperator>("ternary");
  diag(Ternary->getExprLoc(), "indirect function calls not supported");
}

} // namespace hooks
} // namespace tidy
} // namespace clang
