//===--- FloatManipPureCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatManipPureCheck.h"
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
using utils::pure::makeTraceFloatFix;

const char *FloatManipPureCheck::BoundNames[] = { "callExp", "callMan", "callSign" };

template<typename T>
void FloatManipPureCheck::registerCall(MatchFinder *Finder, T CallExpr) {
  Finder->addMatcher(declStmt(has(varDecl(has(CallExpr)).bind("variable"))).bind("declarationStatement"), this);
  Finder->addMatcher(binaryOperator(hasOperatorName("="), hasLHS(declRefExpr().bind("reference")), hasRHS(CallExpr)).bind("assignmentExpression"), this);
  // nested calls get warning w/o fix
  Finder->addMatcher(callExpr(has(CallExpr)), this);
}

void FloatManipPureCheck::registerMatchers(MatchFinder *Finder) {
  const char *FunctionNames[] = { "float_exponent_set", "float_mantissa_set", "float_sign_set" };

  static_assert((sizeof(BoundNames) / sizeof(BoundNames[0])) == (sizeof(FunctionNames) / sizeof(FunctionNames[0])), "function names must correspond to bound names");

  for (size_t i = 0; i < sizeof(BoundNames) / sizeof(BoundNames[0]); ++i)
    registerCall(Finder, callExpr(
      callee(
	functionDecl(hasName(FunctionNames[i])).bind("declaration")),
      hasArgument(0, expr().bind("oldNumber")),
      hasArgument(1, expr().bind("newPiece"))).bind(BoundNames[i]));
}

void FloatManipPureCheck::check(const MatchFinder::MatchResult &Result) {
  const int64_t Minima[] = { -96, 1000000000000000ll, 0 };
  const int64_t Maxima[] = { 80, 9999999999999999ll, 1 };

  static_assert((sizeof(BoundNames) / sizeof(BoundNames[0])) == (sizeof(Minima) / sizeof(Minima[0])), "minima must correspond to bound names");
  static_assert((sizeof(BoundNames) / sizeof(BoundNames[0])) == (sizeof(Maxima) / sizeof(Maxima[0])), "maxima must correspond to bound names");

  for (size_t i = 0; i < sizeof(BoundNames) / sizeof(BoundNames[0]); ++i) {
    const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>(BoundNames[i]);
    if (Call)
      checkCall(Result, *Call, Minima[i], Maxima[i]);
  }
}

void FloatManipPureCheck::checkCall(const MatchFinder::MatchResult &Result, const CallExpr &Call, int64_t Min, int64_t Max) {
  assert(Result.Context);
  ASTContext &Context = *(Result.Context);  
  const Expr *NewPiece = Result.Nodes.getNodeAs<Expr>("newPiece");
  Optional<llvm::APSInt> NewPieceValue = NewPiece->getIntegerConstantExpr(Context);
  if (NewPieceValue) {
    if ((*NewPieceValue < Min) || (*NewPieceValue > Max)) {
      const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
      std::string FunctionName = Matched->getDeclName().getAsString();
      diag(NewPiece->getBeginLoc(), "last argument of %0 must be between %1 and %2") <<
	FunctionName << Min << Max;
      return;
    }

    const Expr *OldNumber = Result.Nodes.getNodeAs<Expr>("oldNumber");
    Optional<llvm::APSInt> OldNumberValue = OldNumber->getIntegerConstantExpr(Context);
    if (OldNumberValue) {
      const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
      std::string FunctionName = Matched->getDeclName().getAsString();
      SourceLocation End;
      llvm::StringRef Name;

      if (const DeclStmt *DeclarationStatement = Result.Nodes.getNodeAs<DeclStmt>("declarationStatement")) {
	auto Parents = Context.getParents(*DeclarationStatement);
	if (Parents.size() == 1) {
	  const CompoundStmt *ParentStatement = Parents[0].get<CompoundStmt>();
	  if (ParentStatement) {
	    End = condSkipDeclaration(DeclarationStatement->getEndLoc(), Context, "trace_num");
	    const VarDecl *Variable = Result.Nodes.getNodeAs<VarDecl>("variable");
	    assert(Variable);
	    Name = Variable->getName();
	  }
	}
      } else if (const BinaryOperator *BinOp = Result.Nodes.getNodeAs<BinaryOperator>("assignmentExpression")) {
	auto Parents = Context.getParents(*BinOp);
	if (Parents.size() == 1) {
	  const CompoundStmt *ParentStatement = Parents[0].get<CompoundStmt>();
	  if (ParentStatement) {
	    End = condSkipStatement(BinOp->getEndLoc(), Context, "trace_num");
	    const DeclRefExpr *Reference = Result.Nodes.getNodeAs<DeclRefExpr>("reference");
	    assert(Reference);
	    const NamedDecl *Declaration = Reference->getDecl();
	    if (Declaration)
	      Name = Declaration->getName();
	  }
	}
      }

      if (End.isValid() && !Name.empty()) {
	std::string Fix = makeTraceFloatFix(Name);
	diag(Call.getBeginLoc(), "output of %0 can be precomputed") <<
	  FunctionName <<
	  FixItHint::CreateInsertion(End, Fix);
      } else {
	diag(Call.getBeginLoc(), "output of %0 can be precomputed") << FunctionName;
      }
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
