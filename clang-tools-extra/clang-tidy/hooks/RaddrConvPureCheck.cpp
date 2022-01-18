//===--- RaddrConvPureCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "RaddrConvPureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

// copied from BracesAroundStatementsCheck.cpp
static tok::TokenKind getTokenKind(SourceLocation Loc, const SourceManager &SM,
				   const ASTContext *Context) {
  Token Tok;
  SourceLocation Beginning =
      Lexer::GetBeginningOfToken(Loc, SM, Context->getLangOpts());
  const bool Invalid =
      Lexer::getRawToken(Beginning, Tok, SM, Context->getLangOpts());
  assert(!Invalid && "Expected a valid token.");

  if (Invalid)
    return tok::NUM_TOKENS;

  return Tok.getKind();
}

static SourceLocation
forwardSkipWhitespaceAndComments(SourceLocation Loc, const SourceManager &SM,
				 const ASTContext *Context) {
  assert(Loc.isValid());
  for (;;) {
    while (isWhitespace(*SM.getCharacterData(Loc)))
      Loc = Loc.getLocWithOffset(1);

    tok::TokenKind TokKind = getTokenKind(Loc, SM, Context);
    if (TokKind != tok::comment)
      return Loc;

    // Fast-forward current token.
    Loc = Lexer::getLocForEndOfToken(Loc, 0, SM, Context->getLangOpts());
  }
}

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
    callExpr(callee(functionDecl(hasName("util_accid")).bind("declaration")),
	     hasArgument(0, cStyleCastExpr(hasDescendant(declRefExpr(to(varDecl().bind("outputBuffer")))))),
	     hasArgument(1, sizeOfExpr(hasDescendant(declRefExpr(to(varDecl().bind("sizedBuffer")))))),
	     hasArgument(2, cStyleCastExpr(hasDescendant(stringLiteral().bind("inputLiteral")))),
	     hasArgument(3, sizeOfExpr(hasDescendant(stringLiteral().bind("sizedLiteral")))));

  Finder->addMatcher(compoundStmt(has(declStmt(has(varDecl(has(CallExpr)))).bind("declarationStatement"))), this);
  Finder->addMatcher(compoundStmt(has(binaryOperator(hasOperatorName("="), hasRHS(CallExpr)).bind("assignmentExpression"))), this);
  Finder->addMatcher(compoundStmt(has(ifStmt(has(binaryOperator(has(CallExpr)))).bind("ifStatement"))), this);
}

void RaddrConvPureCheck::check(const MatchFinder::MatchResult &Result) {
  const StringLiteral *InputLiteral = Result.Nodes.getNodeAs<StringLiteral>("inputLiteral");
  const StringLiteral *SizedLiteral = Result.Nodes.getNodeAs<StringLiteral>("inputLiteral");
  const VarDecl *OutputBuffer = Result.Nodes.getNodeAs<VarDecl>("outputBuffer");
  const VarDecl *SizedBuffer = Result.Nodes.getNodeAs<VarDecl>("sizedBuffer");
  if ((InputLiteral->getString() == SizedLiteral->getString()) && (OutputBuffer->getCanonicalDecl() == SizedBuffer->getCanonicalDecl())) {
    const DeclStmt *DeclarationStatement = Result.Nodes.getNodeAs<DeclStmt>("declarationStatement");
    const BinaryOperator *BinOp = Result.Nodes.getNodeAs<BinaryOperator>("assignmentExpression");
    const IfStmt *IfStatement = Result.Nodes.getNodeAs<IfStmt>("ifStatement");
    const FunctionDecl *Declaration = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
    ASTContext &Context = Declaration->getASTContext();
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
