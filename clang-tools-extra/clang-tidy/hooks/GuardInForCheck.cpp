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
const int MAX_NESTED_LOOP_LEVEL = 7;
const int MAX_LOOP_CONDITIONS = 5;
const int MAX_LOOP_INIT_COMA = 5;

std::string buildStr(std::string str, int nestingLevel) {
  return str + std::to_string(nestingLevel);
}

template<std::size_t... S>
auto anyOfFromVector(const std::vector<StatementMatcher>& vec, std::index_sequence<S...>) {
    return anyOf(vec[S]...);
}

template<std::size_t size>
auto anyOfFromVector(const std::vector<StatementMatcher>& vec) {
    return anyOfFromVector(vec, std::make_index_sequence<size>());
}

auto isUnguarded()  {
  return unless(has(binaryOperator(has(callExpr(callee(functionDecl(hasName("_g"))))))));
}

auto hasGuardCall(int nestingLevel) {
  return compoundStmt(
    hasDescendant(callExpr(
      callee(functionDecl(hasName("_g"))), 
        hasArgument(1, expr().bind(buildStr("guardLimit-", nestingLevel)))
    )),
    unless(eachOf(
      hasAnySubstatement(forStmt(hasCondition(binaryOperator(hasLHS(callExpr(callee(functionDecl(hasName("_g"))))))))), 
      hasAnySubstatement(stmt(hasDescendant(callExpr(callee(functionDecl(hasName("_g"))))))) 
    ))
  ).bind(buildStr("guardCallInBody-", nestingLevel));
}

auto conditionLimit(int nestingLevel) {
  return integerLiteral().bind(buildStr("constLimit-", nestingLevel));
}

auto conditionVarName(int nestingLevel) {
  return ignoringParenImpCasts(declRefExpr(
          to(varDecl(equalsBoundNode(buildStr("initVarName-", nestingLevel))))));
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
            hasRHS(anyOf(conditionVarName(nestingLevel), conditionLimit(nestingLevel)))
          ).bind(buildStr("condOp-", nestingLevel));
}

auto loopBooleanConditions(int nestingLevel) {
  return anyOf(
    lessThanCondition(nestingLevel),
    greaterThanCondition(nestingLevel),
    notEqualCondition(nestingLevel)
  );
}

//loops which are guarded are also processed to properly calculate guard limit for nested loops
auto guardedCondition(int nestingLevel) {
  return callExpr(callee(functionDecl(
      hasName("_g"))),
      hasArgument(1, expr().bind(buildStr("guardLimit-", nestingLevel)))
    ).bind(buildStr("guardedCond-", nestingLevel));
}

template<int level>
auto conditionWithOperator(int nestingLevel) {
  //will match any condition inside for() containing operators: , && || that could be nested up to 5 levels
  return binaryOperator(anyOf(hasOperatorName(","), hasOperatorName("&&"), hasOperatorName("||")),
    anyOf(
      hasLHS(guardedCondition(nestingLevel)),
      hasEitherOperand(anyOf(conditionWithOperator<level + 1>(nestingLevel), loopBooleanConditions(nestingLevel)))
    )
  );
}

template<>
auto conditionWithOperator<MAX_LOOP_CONDITIONS>(int nestingLevel) {
  return binaryOperator(anyOf(hasOperatorName("&&"), hasOperatorName("||")),
    anyOf(
      hasLHS(guardedCondition(nestingLevel)),
      hasEitherOperand(loopBooleanConditions(nestingLevel))
    )
  );
}

//e.g. for(int i = 0;...)
auto loopVarInitInsideLoopExpr(int nestingLevel) {
  const auto decl = varDecl(hasInitializer(ignoringParenImpCasts(expr().bind(buildStr("constInit-", nestingLevel)))))
          .bind(buildStr("initVarName-", nestingLevel));

  //handle cases where 'i' is from 0 up to 4th position in the initialization (e.g. int k = 0, i = 0)
  return declStmt(eachOf(
      containsDeclaration(0, decl),
      containsDeclaration(1, decl),
      containsDeclaration(2, decl),
      containsDeclaration(3, decl),
      containsDeclaration(4, decl)
  ));
}

//e.g. int i; for(i = 0;...)
auto loopVarInitOutsideLoopExpr(int nestingLevel) {
  return binaryOperator(
          hasLHS(declRefExpr(to(varDecl().bind(buildStr("initVarName-", nestingLevel))))),
          hasRHS(ignoringParenImpCasts(integerLiteral().bind(buildStr("constInit-", nestingLevel)))));
}

template<int level>
auto loopInitWithComa(int nestingLevel) {
  return binaryOperator(hasOperatorName(","),
    hasEitherOperand(anyOf(
        loopInitWithComa<level + 1>(nestingLevel),
        loopVarInitInsideLoopExpr(nestingLevel),
        loopVarInitOutsideLoopExpr(nestingLevel)
      ))
    );
}

template<>
auto loopInitWithComa<MAX_LOOP_INIT_COMA>(int nestingLevel) {
  return binaryOperator(hasOperatorName(","),
    hasEitherOperand(anyOf(
      loopVarInitInsideLoopExpr(nestingLevel),
      loopVarInitOutsideLoopExpr(nestingLevel)
    ))
  );
}

auto loopInit(int nestingLevel) {
  return hasLoopInit(
          anyOf(
            loopVarInitInsideLoopExpr(nestingLevel),
            loopVarInitOutsideLoopExpr(nestingLevel),
            loopInitWithComa<0>(nestingLevel)
          )
        );
}

auto loopCondition(int nestingLevel) {
  return hasCondition(
        anyOf(
          conditionWithOperator<0>(nestingLevel),
          loopBooleanConditions(nestingLevel)
        )
      );
}

auto loopIncrement(int nestingLevel) {
  return hasIncrement(
          anyOf(
            unaryOperator(
              anyOf(hasOperatorName("++"), hasOperatorName("--")),
              hasUnaryOperand(declRefExpr(
                to(varDecl(equalsBoundNode(buildStr("initVarName-", nestingLevel)))))
              )
            ),
            //+=, -=, *=, /=, i = i + 1, i = i * 2, etc.
            binaryOperator(
              isAssignmentOperator(),
              hasLHS(declRefExpr(to(varDecl(equalsBoundNode(buildStr("initVarName-", nestingLevel)))))),
              hasRHS(anyOf(
                binaryOperator( //i = i + 1, i = i * 2, etc.
                  hasEitherOperand(integerLiteral().bind(buildStr("incValue-", nestingLevel))), 
                  hasEitherOperand(ignoringParenImpCasts(declRefExpr(to(varDecl(equalsBoundNode(buildStr("initVarName-", nestingLevel)))))))
                ).bind(buildStr("secondIncOp-", nestingLevel)),
                expr().bind(buildStr("incValue-", nestingLevel)) //+=, -=, *=, /=
              ))
            ).bind(buildStr("incOp-", nestingLevel))
        )
      );
}

auto loopBody(int nestingLevel) {
  return hasBody(anyOf(
    hasGuardCall(nestingLevel),
    stmt()
  ));
}

//creates nested loop statement recursively for the specified nesting level
//if one of the nested loops is not "strict" (no init value or no condition) it is bound as "nonStrictNestedLoop"
//in such a case guard limit value will not be calculated and only warning will be displayed without hints
//similar if one of nested loops is while loop - for such we can't calculate the limit too
auto nestedForStmt(int totalNestingLevels, int initialNestingLevel = 1) {
  if (initialNestingLevel == totalNestingLevels) {
    return stmt(anyOf(forStmt(loopInit(totalNestingLevels), loopIncrement(totalNestingLevels), loopCondition(totalNestingLevels), loopBody(totalNestingLevels)),
                      forStmt().bind(buildStr("nonStrictNestedLoop-", initialNestingLevel)),
                      whileStmt().bind(buildStr("whileNestedLoop-", initialNestingLevel))));
  }
  return stmt(anyOf(forStmt(hasAncestor(nestedForStmt(totalNestingLevels, initialNestingLevel + 1)),
                      loopInit(initialNestingLevel), loopIncrement(initialNestingLevel), loopCondition(initialNestingLevel), loopBody(initialNestingLevel)),
                    forStmt(hasAncestor(nestedForStmt(totalNestingLevels, initialNestingLevel + 1))).bind(buildStr("nonStrictNestedLoop-", initialNestingLevel)),
                    whileStmt(hasAncestor(nestedForStmt(totalNestingLevels, initialNestingLevel + 1))).bind(buildStr("whileNestedLoop-", initialNestingLevel))));
}

int sgn(int val) {
    return (0 < val) - (val < 0);
}

Optional<int64_t> calculateLoopGuardLimit(std::string op, int64_t initVal, int64_t condVal, int64_t incVal, bool condContainsEq) {
  if (op.find("+") != std::string::npos && incVal != 0 && sgn(condVal - initVal) == sgn(incVal)) {
    return std::ceil(static_cast<double>(condVal - initVal + (condContainsEq ? 1 : 0)) / static_cast<double>(incVal));
  }
  if (op.find("-") != std::string::npos && incVal != 0 && sgn(initVal - condVal) == sgn(incVal)) {
    return std::ceil(static_cast<double>(initVal - condVal + (condContainsEq ? 1 : 0)) / static_cast<double>(incVal));
  }
  if (op.find("*") != std::string::npos && initVal != 0 && std::abs(incVal) != 1) {
    if (incVal > 0) {
      //this is a solution to eq: (a^x)*b = c
      int64_t x = std::floor(std::log((condVal - initVal + (condContainsEq ? 1 : 0))/initVal) / std::log(incVal)) + 1;
      return x;
    }
    else {
      int64_t x = 0;
      if (condContainsEq) {
        ++condVal;
      }
      //any idea how to calc that without loop?
      for (int i = initVal; i < condVal; i *= incVal) {
        ++x;
      }
      return x;
    }
  }
  if (op.find("/") != std::string::npos && std::abs(incVal) > 1) {
    if (condContainsEq) {
      --condVal;
    }
    if (condVal >= 0)  {
      int64_t x = 0;
      //any idea how to calc that without loop?
      for (int i = initVal; i > condVal; i /= incVal) {
        ++x;
      }
      return x;
    }
  }
  return {};
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
      const auto *NonStrictLoopNestedMatched = Result.Nodes.getNodeAs<Stmt>(buildStr("nonStrictNestedLoop-", i));
      const auto *WhileLoopNestedMatched = Result.Nodes.getNodeAs<Stmt>(buildStr("whileNestedLoop-", i));
      
      if (NonStrictLoopNestedMatched || WhileLoopNestedMatched) {
        //found non strict/while loop, skip guard limit calculation and show only warning without any hint
        Found = false;
        return;
      }
      
      const Expr *ConstInit = Result.Nodes.getNodeAs<Expr>(buildStr("constInit-", i));
      const Expr *ConstLimit = Result.Nodes.getNodeAs<Expr>(buildStr("constLimit-", i));
      const Expr *GuardedCond = Result.Nodes.getNodeAs<Expr>(buildStr("guardedCond-", i));
      const Expr *GuardCallInBody = Result.Nodes.getNodeAs<Expr>(buildStr("guardCallInBody-", i));
      const Expr *IncValue = Result.Nodes.getNodeAs<Expr>(buildStr("incValue-", i));

      const BinaryOperator *CondOp = Result.Nodes.getNodeAs<BinaryOperator>(buildStr("condOp-", i));
      const BinaryOperator *SecondIncOp = Result.Nodes.getNodeAs<BinaryOperator>(buildStr("secondIncOp-", i));
      const BinaryOperator *IncOp = Result.Nodes.getNodeAs<BinaryOperator>(buildStr("incOp-", i));

      int64_t LimitedValue = 0;

      //if for loop has guard use it
      if (GuardedCond || GuardCallInBody) {
        if (GuardedCond) {
          CondBegLoc = GuardedCond->getBeginLoc();
        }

        const Expr *GuardLimitExpr = Result.Nodes.getNodeAs<Expr>(buildStr("guardLimit-", i));
        Optional<llvm::APSInt> GuardLimitValue = GuardLimitExpr->getIntegerConstantExpr(Context);

        if (GuardLimitValue) {
          int ThisIterationGuard = GuardLimitValue->getExtValue();
          LimitedValue = ThisIterationGuard / GuardLimit - 1;
        }
      }
      //else calculate its limit using for condition, init value and increment
      else {
        CondBegLoc = CondOp->getBeginLoc();
        bool CondContainsEq = CondOp->getOpcodeStr().str().find("=") != std::string::npos && 
                                CondOp->getOpcodeStr().str().find("!") == std::string::npos;

        Optional<llvm::APSInt> ConstInitValue = ConstInit->getIntegerConstantExpr(Context);
        Optional<llvm::APSInt> ConstLimitValue = ConstLimit->getIntegerConstantExpr(Context);

        if (ConstInitValue && ConstLimitValue) {
          //increment is e.g. i += 2, i = i * 2, etc.
          if (IncOp) {
            if (!IncValue || !IncValue->getIntegerConstantExpr(Context).hasValue()) {
              Found = false;
              return;
            }

            Optional<llvm::APSInt> IncValueOpt = IncValue->getIntegerConstantExpr(Context);
            std::string Op = SecondIncOp ? SecondIncOp->getOpcodeStr().str() : IncOp->getOpcodeStr().str();

            auto LimitedValueOpt = calculateLoopGuardLimit(Op, ConstInitValue->getExtValue(), ConstLimitValue->getExtValue(), 
                            IncValueOpt->getExtValue(), CondContainsEq);

            if (!LimitedValueOpt) {
              Found = false;
              return;
            }
            LimitedValue = *LimitedValueOpt;
          }
          //increment is a simple i++ or i--
          else {
            llvm::APSInt Value = *ConstLimitValue - *ConstInitValue;
            LimitedValue = std::abs(Value.getExtValue()) + (CondContainsEq ? 1 : 0);
          }
        }
      }
      if (i == 0) {
        LimitedValue++;
      }

      if (LimitedValue < MAX_FOR_LIMIT) {
        GuardLimit *= static_cast<int>(LimitedValue);
        Found = (GuardLimit < MAX_FOR_LIMIT);
      }
      else {
        Found = false;
        return;
      }
    }
  }
};

}

void GuardInForCheck::registerMatchers(MatchFinder *Finder) {
  // based on https://clang.llvm.org/docs/LibASTMatchersTutorial.html

  //matches "standard" loops, e.g.: for (int i = 0; i < 2; i++), for (int i = 0; 2 > i; i++), int i; for (i = 0; i < 2; i++)
  StatementMatcher StrictLoopMatcher =
    forStmt(isUnguarded(),
      isExpansionInMainFile(),
      loopInit(0),
      loopIncrement(0),
      loopCondition(0),
      hasBody(unless(hasGuardCall(0)))
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
        loopCondition(0),
        hasBody(unless(hasGuardCall(0)))
      ).bind(buildStr("nestedLoop-", i))
    );
  }
  StatementMatcher NestedLoopsMatcher = stmt(anyOfFromVector<MAX_NESTED_LOOP_LEVEL>(NestedLoopMatcherArray));

  //matches e.g. for(;;), for(int i = 0; 1; ++i), etc. Basically any "non standard" loop that is not matched by the StrictLoopMatcher
  StatementMatcher AnyLoopMatcher = forStmt(isUnguarded(), hasBody(unless(hasGuardCall(0))), isExpansionInMainFile()).bind("anyLoop");

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
