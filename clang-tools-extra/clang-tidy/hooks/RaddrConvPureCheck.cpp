//===--- RaddrConvPureCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "RaddrConvPureCheck.h"
#include "../utils/LexerUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

using utils::lexer::getTokenKind;
using utils::lexer::forwardSkipWhitespaceAndComments;

static SourceLocation checkLocationForFix(SourceLocation TargetLoc, const ASTContext &Context, const SourceManager &SM) {
  if (!TargetLoc.isValid())
    return TargetLoc;

  SourceLocation EndLoc = Lexer::getLocForEndOfToken(TargetLoc, 0, SM, Context.getLangOpts());
  if (!EndLoc.isValid())
    return TargetLoc;

  const char *Begin = SM.getCharacterData(TargetLoc);
  const char *End = SM.getCharacterData(EndLoc);
  llvm::StringRef TestToken(Begin, End - Begin);
  if (!TestToken.compare("trace")) // fix apparently already applied
    return SourceLocation();

  return TargetLoc;
}

static SourceLocation condSkipDeclaration(SourceLocation Loc, const ASTContext &Context) {
  if (!Loc.isValid())
    return Loc;

  SourceLocation NextLoc = Loc.getLocWithOffset(1);
  if (!NextLoc.isValid())
    return NextLoc;

  const SourceManager &SM = Context.getSourceManager();
  SourceLocation EndLoc = forwardSkipWhitespaceAndComments(NextLoc, SM, &Context);
  return checkLocationForFix(EndLoc, Context, SM);
}

static SourceLocation condSkipStatement(SourceLocation Loc, const ASTContext &Context) {
  SourceLocation InvalidLoc;
  if (!Loc.isValid())
    return InvalidLoc;

  SourceLocation NextLoc = Loc.getLocWithOffset(1);
  if (!NextLoc.isValid())
    return InvalidLoc;

  const SourceManager &SM = Context.getSourceManager();
  SourceLocation SemiLoc = forwardSkipWhitespaceAndComments(NextLoc, SM, &Context);
  if (SemiLoc.isInvalid())
    return InvalidLoc;

  tok::TokenKind TokKind = getTokenKind(SemiLoc, SM, &Context);
  if (TokKind != tok::semi)
    return InvalidLoc;

  SourceLocation AfterLoc = Lexer::getLocForEndOfToken(SemiLoc, 0, SM, Context.getLangOpts());
  if (!AfterLoc.isValid())
    return InvalidLoc;

  SourceLocation EndLoc = AfterLoc.getLocWithOffset(1);
  return checkLocationForFix(EndLoc, Context, SM);
}

static std::string makeFix(llvm::StringRef BufferName) {
  std::string Fix("trace((uint32_t)\"");
  Fix += BufferName;
  Fix += "\", sizeof(\"";
  Fix += BufferName;
  Fix += "\"), (uint32_t)";
  Fix += BufferName;
  Fix += ", sizeof(";
  Fix += BufferName;
  Fix += "), 1);\n";
  return Fix;
}

void RaddrConvPureCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("util_accid"))),
	     hasArgument(0, cStyleCastExpr(hasDescendant(declRefExpr(to(varDecl().bind("outputBuffer")))))),
	     hasArgument(1, expr().bind("bufferSize")),
	     hasArgument(2, cStyleCastExpr(hasDescendant(stringLiteral().bind("inputLiteral")))),
	     hasArgument(3, expr().bind("literalSize")));

  Finder->addMatcher(compoundStmt(has(declStmt(has(varDecl(has(CallExpr)))).bind("declarationStatement"))), this);
  Finder->addMatcher(compoundStmt(has(binaryOperator(hasOperatorName("="), hasRHS(CallExpr)).bind("assignmentExpression"))), this);
  Finder->addMatcher(compoundStmt(has(ifStmt(has(binaryOperator(has(CallExpr)))).bind("ifStatement"))), this);
}

void RaddrConvPureCheck::check(const MatchFinder::MatchResult &Result) {
  const StringLiteral *InputLiteral = Result.Nodes.getNodeAs<StringLiteral>("inputLiteral");
  const Expr *LiteralSize = Result.Nodes.getNodeAs<Expr>("literalSize");
  const VarDecl *OutputBuffer = Result.Nodes.getNodeAs<VarDecl>("outputBuffer");
  const Expr *BufferSize = Result.Nodes.getNodeAs<Expr>("bufferSize");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> LiteralSizeValue = LiteralSize->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> BufferSizeValue = BufferSize->getIntegerConstantExpr(Context);
  if (LiteralSizeValue && BufferSizeValue) {
    const DeclStmt *DeclarationStatement = Result.Nodes.getNodeAs<DeclStmt>("declarationStatement");
    const BinaryOperator *BinOp = Result.Nodes.getNodeAs<BinaryOperator>("assignmentExpression");
    const IfStmt *IfStatement = Result.Nodes.getNodeAs<IfStmt>("ifStatement");
    SourceLocation End;

    if (DeclarationStatement) {
      End = condSkipDeclaration(DeclarationStatement->getEndLoc(), Context);
    } else if (BinOp) {
      End = condSkipStatement(BinOp->getEndLoc(), Context);
    } else if (IfStatement) {
      End = condSkipStatement(IfStatement->getEndLoc(), Context);
    }

    if (End.isValid()) {
      std::string Fix = makeFix(OutputBuffer->getName());
      diag(InputLiteral->getBeginLoc(), "output of util_accid can be precomputed") << FixItHint::CreateInsertion(End, Fix);
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
