//===--- ReserveLimitCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ReserveLimitCheck.h"
#include "../utils/LexerUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

using utils::lexer::getTokenKind;
using utils::lexer::forwardSkipWhitespaceAndComments;

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
  Finder->addMatcher(callExpr(callee(functionDecl(hasName("etxn_reserve")).bind("declaration")), hasArgument(0, expr().bind("argument"))).bind("call"), this);
}

void ReserveLimitCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Argument = Result.Nodes.getNodeAs<Expr>("argument");
  const FunctionDecl *Declaration = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
  ASTContext &Context = Declaration->getASTContext();
  Optional<llvm::APSInt> ArgumentValue = Argument->getIntegerConstantExpr(Context);
  if (ArgumentValue) {
    llvm::APSInt LimitedValue = *ArgumentValue;
    if (!LimitedValue) {
      // call removal is only proposed when the call is a standalone
      // statement (e.g. not inside a bigger expression, if condition
      // etc.)
      const CallExpr *Child = Result.Nodes.getNodeAs<CallExpr>("call");
      auto Parents = Context.getParents(*Child);
      if (Parents.size() == 1) {
	const CompoundStmt *ParentStatement = Parents[0].get<CompoundStmt>();
	if (ParentStatement) {
	  SourceLocation End(condFindSemicolon(Child->getEndLoc(), Context));
	  if (End.isValid()) {
	    SourceRange CallRange(Child->getBeginLoc(), End);
	    diag(Argument->getBeginLoc(), "function etxn_reserve need not be called to reserve 0 transactions") << FixItHint::CreateRemoval(CallRange);
	    return;
	  }
	}
      }

      diag(Argument->getBeginLoc(), "function etxn_reserve need not be called to reserve 0 transactions") << SourceRange(Argument->getBeginLoc(), Argument->getEndLoc());
    } else if (LimitedValue > MAX_EMIT) {
      diag(Argument->getBeginLoc(), "function etxn_reserve may not reserve more than %0 transactions") << MAX_EMIT << SourceRange(Argument->getBeginLoc(), Argument->getEndLoc());
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
