//===--- ReserveLimitCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ReserveLimitCheck.h"
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

static SourceLocation condFindSemicolon(SourceLocation Loc, const ASTContext &Context) {
  if (!Loc.isValid())
    return Loc;

  SourceLocation NextLoc = Loc.getLocWithOffset(1);
  if (!NextLoc.isValid())
    return Loc;

  const SourceManager &SM = Context.getSourceManager();
  SourceLocation SemiLoc = forwardSkipWhitespaceAndComments(NextLoc, SM, &Context);
  if (SemiLoc.isInvalid())
    return Loc;

  tok::TokenKind TokKind = getTokenKind(SemiLoc, SM, &Context);
  if (TokKind != tok::semi)
    return Loc;

  SourceLocation EndLoc = Lexer::getLocForEndOfToken(SemiLoc, 0, SM, Context.getLangOpts());
  if (EndLoc.isInvalid())
    return Loc;

  return EndLoc;
}

void ReserveLimitCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(callExpr(callee(functionDecl(hasName("etxn_reserve")).bind("declaration")), hasArgument(0, integerLiteral().bind("constReserve"))).bind("call"), this);
}

void ReserveLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const IntegerLiteral *ConstReserve = Result.Nodes.getNodeAs<IntegerLiteral>("constReserve");
  llvm::APInt Value = ConstReserve->getValue();
  uint64_t LimitedValue = Value.getLimitedValue();
  if (!LimitedValue) {
    // call removal is only proposed when the call is a standalone
    // statement (e.g. not inside a bigger expression, if
    // condition etc.)
    const FunctionDecl *Declaration = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
    const CallExpr *Child = Result.Nodes.getNodeAs<CallExpr>("call");
    ASTContext &Context = Declaration->getASTContext();
    auto Parents = Context.getParents(*Child);
    if (Parents.size() == 1) {
      const CompoundStmt *ParentStatement = Parents[0].get<CompoundStmt>();
      if (ParentStatement) {
	SourceLocation End(condFindSemicolon(Child->getEndLoc(), Context));
	if (End.isValid()) {
	  SourceRange CallRange(Child->getBeginLoc(), End);
	  diag(ConstReserve->getLocation(), "etxn_reserve need not be called to reserve 0 transactions") << FixItHint::CreateRemoval(CallRange);
	  return;
	}
      }
    }

    diag(ConstReserve->getLocation(), "etxn_reserve need not be called to reserve 0 transactions");
  } else if (LimitedValue > MAX_EMIT) {
    diag(ConstReserve->getLocation(), "etxn_reserve may not reserve more than %0 transactions") << MAX_EMIT;
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
