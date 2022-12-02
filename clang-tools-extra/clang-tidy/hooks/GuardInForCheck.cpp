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
const int MAX_NESTED_LOOP_LEVEL = 10;

bool areSameVariable(const ValueDecl *First, const ValueDecl *Second) {
  return First && Second &&
    First->getCanonicalDecl() == Second->getCanonicalDecl();
}

auto isUnguarded()  {
  return unless(has(binaryOperator(has(callExpr(callee(functionDecl(hasName("_g"))))))));
}

std::string buildStr(std::string str, int nestingLevel) {
  return str + std::to_string(nestingLevel);
}

auto conditionLimit(int nestingLevel) {
  return expr().bind(buildStr("constLimit-", nestingLevel));
}

auto conditionVarName(int nestingLevel) {
  return ignoringParenImpCasts(declRefExpr(
          to(varDecl(hasType(isInteger())).bind(buildStr("condVarName-", nestingLevel)))));
}

auto lessThanCondition(int nestingLevel) {
  return binaryOperator(anyOf(hasOperatorName("<"), hasOperatorName("<=")),    //e.g. i < 2 or 2 < i
            hasLHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel))),
            hasRHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel)))
          ).bind(buildStr("condOp-", nestingLevel));
}

auto greaterThanCondition(int nestingLevel) {
  return binaryOperator(anyOf(hasOperatorName(">"), hasOperatorName(">=")),   //e.g. 2 > i or i > 2
            hasLHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel))),
            hasRHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel)))
          ).bind(buildStr("condOp-", nestingLevel));
}

auto notEqualCondition(int nestingLevel) {
  return binaryOperator(hasOperatorName("!="),    //e.g. i != 2 or 2 != i
            hasLHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel))),
            hasRHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel)))).bind(buildStr("condOp-", nestingLevel));
}

auto conditionHasGuard(int nestingLevel) {
  return binaryOperator(hasOperatorName(","),
    hasEitherOperand(callExpr(callee(functionDecl(
      hasName("_g"))),
      hasArgument(1, expr().bind(buildStr("constLimit-", nestingLevel))))
    )
  ).bind(buildStr("guarded-", nestingLevel));
}

auto loopCondition(int nestingLevel) {
  return hasCondition(
        anyOf(
          conditionHasGuard(nestingLevel),    //catch loop that is guarded, this is for nested loops only - even if one of ancestor loops is guarded,
                                              //it's condition limit still need to be used (together with limits of all other ancestors)
                                              //to calculate GUARD limit for the most descendant loop
          lessThanCondition(nestingLevel),
          greaterThanCondition(nestingLevel),
          notEqualCondition(nestingLevel)
        )
      );
}

auto loopIncrement(int nestingLevel) {
  return hasIncrement(
          unaryOperator(
            anyOf(hasOperatorName("++"), hasOperatorName("--")),
            hasUnaryOperand(declRefExpr(
              to(varDecl(hasType(isInteger())).bind(buildStr("incVarName-", nestingLevel))))
            )
          )
        );
}

auto loopInit(int nestingLevel) {
  //e.g. for(int i = 0;...)
  const auto LoopVarInitInsideLoopExpr = declStmt(
    hasSingleDecl(varDecl(hasInitializer(expr().bind(buildStr("constInit-", nestingLevel))))
          .bind(buildStr("initVarName-", nestingLevel))));

  //e.g. int i; for(i = 0;...)
  const auto LoopVarInitOutsideLoopExpr = binaryOperator(
          hasLHS(declRefExpr(to(varDecl().bind(buildStr("initVarName-", nestingLevel))))),
          hasRHS(ignoringParenImpCasts(integerLiteral().bind(buildStr("constInit-", nestingLevel)))));

  return hasLoopInit(
          anyOf(
            LoopVarInitInsideLoopExpr,
            LoopVarInitOutsideLoopExpr
          )
        );
}

//creates nested loop statement recursively for the specified nesting level
//if one of the nested loops is not "strict" (no init value or no condition) it is bound as "nonStrictNestedLoop"
//in such a case guard limit value will not be calculated and only warning will be displayed without hints
//similar if one of nested loops is while loop - for such we can't calculate the limit too
auto nestedForStmt(int totalNestingLevels, int initialNestingLevel = 1) {
  if (initialNestingLevel == totalNestingLevels) {
    return stmt(anyOf(forStmt(loopInit(totalNestingLevels), loopIncrement(totalNestingLevels), loopCondition(totalNestingLevels)),
                      forStmt().bind(buildStr("nonStrictNestedLoop-", initialNestingLevel)),
                      whileStmt().bind(buildStr("whileNestedLoop-", initialNestingLevel))));
  }
  return stmt(anyOf(forStmt(hasAncestor(nestedForStmt(totalNestingLevels, initialNestingLevel + 1)),
                      loopInit(initialNestingLevel), loopIncrement(initialNestingLevel), loopCondition(initialNestingLevel)),
                    forStmt(hasAncestor(nestedForStmt(totalNestingLevels, initialNestingLevel + 1))).bind(buildStr("nonStrictNestedLoop-", initialNestingLevel)),
                    whileStmt(hasAncestor(nestedForStmt(totalNestingLevels, initialNestingLevel + 1))).bind(buildStr("whileNestedLoop-", initialNestingLevel))));
}

template<std::size_t... S>
auto anyOfFromVector(const std::vector<StatementMatcher>& vec, std::index_sequence<S...>) {
    return anyOf(vec[S]...);
}

template<std::size_t size>
auto anyOfFromVector(const std::vector<StatementMatcher>& vec) {
    return anyOfFromVector(vec, std::make_index_sequence<size>());
}

void fixLimitForSpecialCases(int64_t &limitedValue, const BinaryOperator *condOp, bool isGuarded, int nestingIndex) {
  if (condOp && 
      (condOp->getOpcodeStr().str() == "<=" || condOp->getOpcodeStr().str() == ">=")) {
    limitedValue++;
  }
  if (isGuarded) {
    //guard value is always number of iterations + 1
    limitedValue--;
  }
  //increase guard limit by 1 for most descendant loop
  if (nestingIndex == 0) {
    ++limitedValue;
  }
}

class LoopHandler {
public:

  int GuardLimit;
  bool Found = false;
  SourceLocation CondBegLoc;

  void processLoop(const ast_matchers::MatchFinder::MatchResult &Result, int nestingLevel) {
    GuardLimit = 1;
    ASTContext &Context = *(Result.Context);

    for (int i = nestingLevel; i >= 0; --i) {
      bool LoopProcessedProperly = false;
      const auto *NonStrictLoopNestedMatched = Result.Nodes.getNodeAs<Stmt>(buildStr("nonStrictNestedLoop-", i));
      const auto *WhileLoopNestedMatched = Result.Nodes.getNodeAs<Stmt>(buildStr("whileNestedLoop-", i));
      if (NonStrictLoopNestedMatched || WhileLoopNestedMatched) {
        //found non strict/while loop, skip guard limit calculation and show only warning without any hint
        Found = false;
        return;
      }

      const Expr *ConstInit = Result.Nodes.getNodeAs<Expr>(buildStr("constInit-", i));
      const VarDecl *IncVar = Result.Nodes.getNodeAs<VarDecl>(buildStr("incVarName-", i));
      const VarDecl *CondVar = Result.Nodes.getNodeAs<VarDecl>(buildStr("condVarName-", i));
      const VarDecl *InitVar = Result.Nodes.getNodeAs<VarDecl>(buildStr("initVarName-", i));
      const BinaryOperator *CondOp = Result.Nodes.getNodeAs<BinaryOperator>(buildStr("condOp-", i));
      const Expr *ConstLimit = Result.Nodes.getNodeAs<Expr>(buildStr("constLimit-", i));
      const Expr *Guarded = Result.Nodes.getNodeAs<Expr>(buildStr("guarded-", i));

      if (Guarded) {
          CondBegLoc = Guarded->getBeginLoc();
      } else {
        CondBegLoc = CondOp->getBeginLoc();
      }

      if ((Guarded || areSameVariable(IncVar, CondVar)) && areSameVariable(IncVar, InitVar)) {
        Optional<llvm::APSInt> ConstInitValue = ConstInit->getIntegerConstantExpr(Context);
        Optional<llvm::APSInt> ConstLimitValue = ConstLimit->getIntegerConstantExpr(Context);

        if (ConstInitValue && ConstLimitValue) {
          llvm::APSInt Value = *ConstLimitValue - *ConstInitValue;
          int64_t LimitedValue = std::abs(Value.getExtValue());

          fixLimitForSpecialCases(LimitedValue, CondOp, Guarded, i);
          
          if (LimitedValue < MAX_FOR_LIMIT) {
            GuardLimit *= static_cast<int>(LimitedValue);
            Found = LoopProcessedProperly = (GuardLimit < MAX_FOR_LIMIT);
          }
        }
      }
      if (!LoopProcessedProperly) {
        Found = false;
        return;
      }
    }
  }
};

}

void GuardInForCheck::registerMatchers(MatchFinder *Finder) {
  // based on https://clang.llvm.org/docs/LibASTMatchersTutorial.htmls

  //matches "standard" loops, e.g.: for (int i = 0; i < 2; i++), for (int i = 0; 2 > i; i++), int i; for (i = 0; i < 2; i++)
  StatementMatcher StrictLoopMatcher =
    forStmt(isUnguarded(),
      isExpansionInMainFile(),
      loopInit(0),
      loopIncrement(0),
      loopCondition(0)
    ).bind("strictLoop");

  //matches nested loops (e.g. for (...) { for(...)} ) up to 10 nesting level
  std::vector<StatementMatcher> NestedLoopMatcherArray;
  NestedLoopMatcherArray.reserve(MAX_NESTED_LOOP_LEVEL);
  for (int i = MAX_NESTED_LOOP_LEVEL; i >= 1; --i) {
    NestedLoopMatcherArray.push_back(forStmt(
        isUnguarded(), 
        isExpansionInMainFile(),
        hasAncestor(nestedForStmt(i)),
        loopInit(0),
        loopIncrement(0),
        loopCondition(0)
      ).bind(buildStr("nestedLoop-", i))
    );
  }
  StatementMatcher NestedLoopsMatcher = stmt(anyOfFromVector<MAX_NESTED_LOOP_LEVEL>(NestedLoopMatcherArray));

  //matches e.g. for(;;), for(int i = 0; 1; ++i), etc. Basically any "non standard" loop that is not matched by the StrictLoopMatcher
  StatementMatcher AnyLoopMatcher = forStmt(isUnguarded(), isExpansionInMainFile()).bind("anyLoop");

  Finder->addMatcher(stmt(anyOf(NestedLoopsMatcher, StrictLoopMatcher, AnyLoopMatcher)).bind("unguarded"), this);
}

void GuardInForCheck::check(const MatchFinder::MatchResult &Result) {
  assert(Result.Context);

  const auto *UngardedLoopMatched = Result.Nodes.getNodeAs<Stmt>("unguarded");
  assert(UngardedLoopMatched);

  const auto *StrictLoopMatched = Result.Nodes.getNodeAs<ForStmt>("strictLoop");
  const auto *AnyLoopMatched = Result.Nodes.getNodeAs<ForStmt>("anyLoop");
  
  SourceLocation CondEndLoc = UngardedLoopMatched->getEndLoc();

  LoopHandler Handler;
  bool FoundNestedLoop = false;
  for (int i = MAX_NESTED_LOOP_LEVEL; i >= 1; --i) {
    const auto *NestedLoopMatched = Result.Nodes.getNodeAs<ForStmt>("nestedLoop-" + std::to_string(i));
    if (NestedLoopMatched) {
      CondEndLoc = NestedLoopMatched->getRParenLoc();
      Handler.processLoop(Result, i);
      FoundNestedLoop = true;
      break;
    }
  }

  if (!FoundNestedLoop) {
    if (StrictLoopMatched) {
      Handler.processLoop(Result, 0);
      CondEndLoc = StrictLoopMatched->getRParenLoc();
    } else if (AnyLoopMatched) {
      CondEndLoc = AnyLoopMatched->getRParenLoc();
    }
  }

  if (Handler.Found) {
      //GUARD macro adds 1 to iterations
      std::string Fix("GUARD(" + std::to_string(Handler.GuardLimit - 1) + "), ");

      diag(Handler.CondBegLoc, "for loop does not call 'GUARD'") <<
        SourceRange(UngardedLoopMatched->getBeginLoc(), CondEndLoc) <<
        FixItHint::CreateInsertion(Handler.CondBegLoc, Fix);
  } else {
    diag(UngardedLoopMatched->getBeginLoc(), "for loop does not call 'GUARD'") <<
      SourceRange(UngardedLoopMatched->getBeginLoc(), CondEndLoc);
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
