//===--- FloatPureCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatPureCheck.h"
#include "../utils/PureUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <string>
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

using utils::pure::condSkipDeclaration;
using utils::pure::condSkipStatement;
using utils::pure::makeTraceFloatFix;

void FloatPureCheck::registerMatchers(MatchFinder *Finder) {
  const auto CallExpr =
    callExpr(callee(functionDecl(hasName("float_set"))),
	     hasArgument(0, expr().bind("exponent")),
	     hasArgument(1, expr().bind("mantissa"))).bind("call");

  // matching all calls w/ 0 args (which don't need any specific
  // parent for a fix replacing them w/ 0) would probably need
  // checking the ancestors manually (like in ReserveLimitCheck)...
  Finder->addMatcher(compoundStmt(has(declStmt(has(varDecl(has(CallExpr)).bind("variable"))).bind("declarationStatement"))), this);
  Finder->addMatcher(compoundStmt(has(binaryOperator(hasOperatorName("="), hasLHS(declRefExpr().bind("reference")), hasRHS(CallExpr)).bind("assignmentExpression"))), this);
}

void FloatPureCheck::check(const MatchFinder::MatchResult &Result) {
  const Expr *Exponent = Result.Nodes.getNodeAs<Expr>("exponent");
  const Expr *Mantissa = Result.Nodes.getNodeAs<Expr>("mantissa");

  assert(Result.Context);
  ASTContext &Context = *(Result.Context);
  Optional<llvm::APSInt> ExponentValue = Exponent->getIntegerConstantExpr(Context);
  Optional<llvm::APSInt> MantissaValue = Mantissa->getIntegerConstantExpr(Context);
  if (ExponentValue && MantissaValue) {
    const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>("call");
    if (!*ExponentValue && !*MantissaValue) {
      SourceRange CallRange(Call->getBeginLoc(), Call->getEndLoc());
      // doc says Special case: XFL canonical 0 is also 0 in the enclosing number.
      diag(Call->getBeginLoc(), "float_set(0, 0) is 0") << FixItHint::CreateReplacement(CallRange, "0");
      return;
    }

    SourceLocation End;
    llvm::StringRef Name;

    if (const DeclStmt *DeclarationStatement = Result.Nodes.getNodeAs<DeclStmt>("declarationStatement")) {
      End = condSkipDeclaration(DeclarationStatement->getEndLoc(), Context, "trace_num");
      const VarDecl *Variable = Result.Nodes.getNodeAs<VarDecl>("variable");
      assert(Variable);
      Name = Variable->getName();
    } else if (const BinaryOperator *BinOp = Result.Nodes.getNodeAs<BinaryOperator>("assignmentExpression")) {
      End = condSkipStatement(BinOp->getEndLoc(), Context, "trace_num");
      const DeclRefExpr *Reference = Result.Nodes.getNodeAs<DeclRefExpr>("reference");
      assert(Reference);
      const NamedDecl *Declaration = Reference->getDecl();
      if (Declaration)
	Name = Declaration->getName();
    }

    if (End.isValid() && !Name.empty()) {
      std::string Fix = makeTraceFloatFix(Name);
      diag(Call->getBeginLoc(), "output of float_set can be precomputed") << FixItHint::CreateInsertion(End, Fix);
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
