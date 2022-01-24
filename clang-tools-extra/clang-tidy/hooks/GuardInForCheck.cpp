//===--- GuardInForCheck.cpp - clang-tidy ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "GuardInForCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

namespace {

const int64_t MAX_FOR_LIMIT = 10000;

bool areSameVariable(const ValueDecl *First, const ValueDecl *Second) {
  return First && Second &&
    First->getCanonicalDecl() == Second->getCanonicalDecl();
}

class InnerCallback : public MatchFinder::MatchCallback {
public:
  bool Found;
  bool FoundCond;
  int GuardLimit;
  SourceLocation CondLoc;

  InnerCallback():Found(false), FoundCond(false), GuardLimit(0) { }

  virtual void run(const MatchFinder::MatchResult &Result) override {
    const Expr *ConstInit = Result.Nodes.getNodeAs<Expr>("constInit");
    const VarDecl *IncVar = Result.Nodes.getNodeAs<VarDecl>("incVarName");
    const VarDecl *CondVar = Result.Nodes.getNodeAs<VarDecl>("condVarName");
    const VarDecl *InitVar = Result.Nodes.getNodeAs<VarDecl>("initVarName");
    const BinaryOperator *CondOp = Result.Nodes.getNodeAs<BinaryOperator>("condOp");
    const Expr *ConstLimit = Result.Nodes.getNodeAs<Expr>("constLimit");
    CondLoc = CondOp->getBeginLoc();
    FoundCond = true;

    if (areSameVariable(IncVar, CondVar) && areSameVariable(IncVar, InitVar)) {
      assert(Result.Context);
      ASTContext &Context = *(Result.Context);
      Optional<llvm::APSInt> ConstInitValue = ConstInit->getIntegerConstantExpr(Context);
      Optional<llvm::APSInt> ConstLimitValue = ConstLimit->getIntegerConstantExpr(Context);
      if (ConstInitValue && ConstLimitValue) {
	llvm::APSInt Value = *ConstLimitValue - *ConstInitValue;
	int64_t LimitedValue = Value.getExtValue();
	if ((0 < LimitedValue) && LimitedValue < MAX_FOR_LIMIT) {
	  GuardLimit = static_cast<int>(LimitedValue) + 1;
	  Found = true;
	}
      }
    }
  }

  std::string getFix() const {
    std::string Fix("_g(__LINE__, ");
    Fix += std::to_string(GuardLimit);
    Fix += "), ";
    return Fix;
  }
};

}

void GuardInForCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(forStmt(unless(has(binaryOperator(has(callExpr(callee(functionDecl(hasName("_g"))))))))).bind("unguarded"), this);
}

void GuardInForCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<ForStmt>("unguarded");
  assert(Matched);

  // based on https://clang.llvm.org/docs/LibASTMatchersTutorial.html
  StatementMatcher LoopMatcher =
    forStmt(isExpansionInMainFile(),
	    hasLoopInit(declStmt(
		hasSingleDecl(varDecl(hasInitializer(expr().bind("constInit")))
				  .bind("initVarName")))),
	    hasIncrement(unaryOperator(
		hasOperatorName("++"),
		hasUnaryOperand(declRefExpr(
		    to(varDecl(hasType(isInteger())).bind("incVarName")))))),
	    hasCondition(binaryOperator(
		hasOperatorName("<"),
		hasLHS(ignoringParenImpCasts(declRefExpr(
		    to(varDecl(hasType(isInteger())).bind("condVarName"))))),
		hasRHS(expr().bind("constLimit"))).bind("condOp")));

  MatchFinder Finder;
  InnerCallback StrictMatch;
  Finder.addMatcher(LoopMatcher, &StrictMatch);
  Finder.matchAST(*(Result.Context));

  if (StrictMatch.Found) {
    assert(StrictMatch.FoundCond);
    diag(StrictMatch.CondLoc, "for loop does not call '_g'") <<
      SourceRange(Matched->getBeginLoc(), Matched->getRParenLoc()) <<
      FixItHint::CreateInsertion(StrictMatch.CondLoc, StrictMatch.getFix());
  } else {
    SourceLocation Loc = StrictMatch.FoundCond ? StrictMatch.CondLoc : Matched->getBeginLoc();
    diag(Loc, "for loop does not call '_g'") <<
      SourceRange(Matched->getBeginLoc(), Matched->getRParenLoc());
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
