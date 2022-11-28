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

}

void GuardInForCheck::registerMatchers(MatchFinder *Finder) {
  // based on https://clang.llvm.org/docs/LibASTMatchersTutorial.html
  const auto CondVarNameExpr = ignoringParenImpCasts(declRefExpr(
          to(varDecl(hasType(isInteger())).bind("condVarName"))));

  const auto CondLimitExpr = expr().bind("constLimit");

  //for(int i = 0;...)
  const auto LoopVarInitInsideLoopExpr = declStmt(
    hasSingleDecl(varDecl(hasInitializer(expr().bind("constInit")))
          .bind("initVarName")));

  //int i; for(i = 0;...)
  const auto LoopVarInitOutsideLoopExpr = binaryOperator(
          hasLHS(declRefExpr(to(varDecl().bind("initVarName")))),
          hasRHS(ignoringParenImpCasts(integerLiteral().bind("constInit")))); 

  const auto UnguardedExpr = unless(has(binaryOperator(has(callExpr(callee(functionDecl(hasName("_g"))))))));

  //matches "standard" loops, e.g.: for (int i = 0; i < 2; i++), for (int i = 0; 2 > i; i++), int i; for (i = 0; i < 2; i++)
  StatementMatcher StrictLoopMatcher =
    forStmt(UnguardedExpr,
      isExpansionInMainFile(),
      hasLoopInit(
        anyOf(
          LoopVarInitInsideLoopExpr,  
          LoopVarInitOutsideLoopExpr
      )),
      hasIncrement(unaryOperator(
        hasOperatorName("++"),
        hasUnaryOperand(declRefExpr(
          to(varDecl(hasType(isInteger())).bind("incVarName")))))),
      hasCondition(
        anyOf(
          binaryOperator(hasOperatorName("<"),    // i < 2
            hasLHS(CondVarNameExpr),
            hasRHS(CondLimitExpr)).bind("condOp"),
          binaryOperator(hasOperatorName(">"),    // 2 > i
            hasLHS(CondLimitExpr),
            hasRHS(CondVarNameExpr)).bind("condOp")
        )
      )
    ).bind("standardLoop");

  //matches e.g. for(;;), for(int i = 0; 1; ++i), etc. Basically any "non standard" loop that is not matched by the StandardLoopMatcher
  StatementMatcher AnyLoopMatcher = forStmt(UnguardedExpr, isExpansionInMainFile()).bind("anyLoop");

  Finder->addMatcher(stmt(anyOf(StrictLoopMatcher, AnyLoopMatcher)).bind("unguarded"), this);
}

void GuardInForCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *UngardedLoopMatched = Result.Nodes.getNodeAs<Stmt>("unguarded");
  assert(UngardedLoopMatched);

  const auto *StandardLoopMatched = Result.Nodes.getNodeAs<ForStmt>("standardLoop");
  const auto *AnyLoopMatched = Result.Nodes.getNodeAs<ForStmt>("anyLoop");

  bool FoundCond = false;
  int GuardLimit = 0;
  SourceLocation CondBegLoc = UngardedLoopMatched->getBeginLoc();
  SourceLocation CondEndLoc = UngardedLoopMatched->getEndLoc();

  if (StandardLoopMatched) {  
    const Expr *ConstInit = Result.Nodes.getNodeAs<Expr>("constInit");  
    const VarDecl *IncVar = Result.Nodes.getNodeAs<VarDecl>("incVarName");
    const VarDecl *CondVar = Result.Nodes.getNodeAs<VarDecl>("condVarName");
    const VarDecl *InitVar = Result.Nodes.getNodeAs<VarDecl>("initVarName");
    const BinaryOperator *CondOp = Result.Nodes.getNodeAs<BinaryOperator>("condOp");
    const Expr *ConstLimit = Result.Nodes.getNodeAs<Expr>("constLimit");
    CondBegLoc = CondOp->getBeginLoc();
    CondEndLoc = CondOp->getEndLoc();    

    assert(Result.Context);
      ASTContext &Context = *(Result.Context);

    if (areSameVariable(IncVar, CondVar) && areSameVariable(IncVar, InitVar)) {
      
      Optional<llvm::APSInt> ConstInitValue = ConstInit->getIntegerConstantExpr(Context);
      Optional<llvm::APSInt> ConstLimitValue = ConstLimit->getIntegerConstantExpr(Context);
      
      if (ConstInitValue && ConstLimitValue) {
        llvm::APSInt Value = *ConstLimitValue - *ConstInitValue;
        int64_t LimitedValue = Value.getExtValue();
        if ((0 < LimitedValue) && LimitedValue < MAX_FOR_LIMIT) {
          GuardLimit = static_cast<int>(LimitedValue) + 1;
          FoundCond = true;
        }
      }
    }
  } else if (AnyLoopMatched) {
    CondEndLoc = AnyLoopMatched->getRParenLoc();
  }

  if (FoundCond) {
      std::string Fix("_g(__LINE__, " + std::to_string(GuardLimit) + "), ");

      diag(CondBegLoc, "for loop does not call '_g'") <<
        SourceRange(CondBegLoc, CondEndLoc) <<
        FixItHint::CreateInsertion(CondBegLoc, Fix);
  } else {
    diag(CondBegLoc, "for loop does not call '_g'") <<
      SourceRange(CondBegLoc, CondEndLoc);
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
