//===--- AccountConvPureCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AccountConvPureCheck.h"
#include "../utils/PureUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

using utils::pure::condSkipDeclaration;
using utils::pure::condSkipStatement;
using utils::pure::makeTraceFix;

void AccountConvPureCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_raddr"))),
	     hasArgument(0, declRefExpr(to(varDecl().bind("outputBuffer")))),
	     hasArgument(1, expr().bind("outputSize")),
	     hasArgument(2, declRefExpr(to(varDecl(hasType(isConstQualified()))))),
	     hasArgument(3, expr().bind("inputSize"))).bind("call");

  Finder->addMatcher(compoundStmt(has(declStmt(has(varDecl(has(CallExpr)))).bind("declarationStatement"))), this);
  Finder->addMatcher(compoundStmt(has(binaryOperator(hasOperatorName("="), hasRHS(CallExpr)).bind("assignmentExpression"))), this);
  Finder->addMatcher(compoundStmt(has(ifStmt(has(binaryOperator(has(CallExpr)))).bind("ifStatement"))), this);
  Finder->addMatcher(compoundStmt(has(CallExpr)), this);
}

void AccountConvPureCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *InputSize = Result.Nodes.getNodeAs<Expr>("inputSize");
  const VarDecl *OutputBuffer = Result.Nodes.getNodeAs<VarDecl>("outputBuffer");
  const Expr *OutputSize = Result.Nodes.getNodeAs<Expr>("outputSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> InputSizeValue = InputSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> OutputSizeValue = OutputSize->getIntegerConstantExpr(Context);
  if (InputSizeValue && OutputSizeValue) {
    const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>("call");
    SourceLocation End;

    if (const DeclStmt *DeclarationStatement = Result.Nodes.getNodeAs<DeclStmt>("declarationStatement")) {
      End = condSkipDeclaration(DeclarationStatement->getEndLoc(), Context);
    } else if (const BinaryOperator *BinOp = Result.Nodes.getNodeAs<BinaryOperator>("assignmentExpression")) {
      End = condSkipStatement(BinOp->getEndLoc(), Context);
    } else if (const IfStmt *IfStatement = Result.Nodes.getNodeAs<IfStmt>("ifStatement")) {
      End = condSkipStatement(IfStatement->getEndLoc(), Context);
    } else {
      End = condSkipStatement(Call->getEndLoc(), Context);
    }

    if (End.isValid()) {
      std::string Fix = makeTraceFix(OutputBuffer->getName(), 0);
      diag(Call->getBeginLoc(), "output of util_raddr can be precomputed") << FixItHint::CreateInsertion(End, Fix);
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
