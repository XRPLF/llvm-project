//===--- RaddrConvPureCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "RaddrConvPureCheck.h"
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

void RaddrConvPureCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_accid"))),
	     hasArgument(0, cStyleCastExpr(hasDescendant(declRefExpr(to(varDecl().bind("outputBuffer")))))),
	     hasArgument(1, expr().bind("bufferSize")),
	     hasArgument(2, cStyleCastExpr(hasDescendant(stringLiteral()))),
	     hasArgument(3, expr().bind("literalSize"))).bind("call");

  Finder->addMatcher(compoundStmt(has(declStmt(has(varDecl(has(CallExpr)))).bind("declarationStatement"))), this);
  Finder->addMatcher(compoundStmt(has(binaryOperator(hasOperatorName("="), hasRHS(CallExpr)).bind("assignmentExpression"))), this);
  Finder->addMatcher(compoundStmt(has(ifStmt(has(binaryOperator(has(CallExpr)))).bind("ifStatement"))), this);
  Finder->addMatcher(compoundStmt(has(CallExpr)), this);
}

void RaddrConvPureCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *LiteralSize = Result.Nodes.getNodeAs<Expr>("literalSize");
  const VarDecl *OutputBuffer = Result.Nodes.getNodeAs<VarDecl>("outputBuffer");
  const Expr *BufferSize = Result.Nodes.getNodeAs<Expr>("bufferSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> LiteralSizeValue = LiteralSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> BufferSizeValue = BufferSize->getIntegerConstantExpr(Context);
  if (LiteralSizeValue && BufferSizeValue) {
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
      std::string Fix = makeTraceFix(OutputBuffer->getName(), 1);
      diag(Call->getBeginLoc(), "output of util_accid can be precomputed") << FixItHint::CreateInsertion(End, Fix);
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
