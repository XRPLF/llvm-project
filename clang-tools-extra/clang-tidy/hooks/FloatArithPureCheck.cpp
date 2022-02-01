//===--- FloatArithPureCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FloatArithPureCheck.h"
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

template<typename T>
void FloatArithPureCheck::registerCall(MatchFinder *Finder, T CallExpr) {
  Finder->addMatcher(declStmt(has(varDecl(has(CallExpr)).bind("variable"))).bind("declarationStatement"), this);
  Finder->addMatcher(binaryOperator(hasOperatorName("="), hasLHS(declRefExpr().bind("reference")), hasRHS(CallExpr)).bind("assignmentExpression"), this);
  // nested calls get warning w/o fix
  Finder->addMatcher(callExpr(has(CallExpr)), this);
}

void FloatArithPureCheck::registerMatchers(MatchFinder *Finder) {
  registerCall(Finder, callExpr(
    callee(
      functionDecl(hasAnyName("float_negate", "float_exponent", "float_mantissa", "float_sign")).bind("declaration")),
    hasArgument(0, expr().bind("para"))).bind("call1"));

  registerCall(Finder, callExpr(
    callee(
      functionDecl(hasName("float_invert")).bind("declaration")),
    hasArgument(0, expr().bind("para"))).bind("callInvert"));

  registerCall(Finder, callExpr(
    callee(
      functionDecl(hasAnyName("float_multiply", "float_sum")).bind("declaration")),
    hasArgument(0, expr().bind("para")),
    hasArgument(1, expr().bind("parb"))).bind("call2"));

  registerCall(Finder, callExpr(
    callee(
      functionDecl(hasName("float_divide")).bind("declaration")),
    hasArgument(0, expr().bind("para")),
    hasArgument(1, expr().bind("parb"))).bind("callDiv"));

  registerCall(Finder, callExpr(
    callee(
      functionDecl(hasName("float_mulratio")).bind("declaration")),
    hasArgument(0, expr().bind("para")),
    hasArgument(1, expr().bind("parb")),
    hasArgument(2, expr().bind("parc")),
    hasArgument(3, expr().bind("pard"))).bind("callRatio"));
}

void FloatArithPureCheck::check(const MatchFinder::MatchResult &Result) {
  const char *GenCallName[] = { "call1", "call2" };
  const size_t GenCallN[] = { 1, 2 };
  const char *DivCallName[] = { "callInvert", "callDiv", "callRatio" };
  const size_t DivCallN[] = { 1, 2, 4 };

  for (size_t i = 0; i < sizeof(GenCallName) / sizeof(GenCallName[0]); ++i) {
    const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>(GenCallName[i]);
    if (Call)
      checkCall(Result, *Call, GenCallN[i]);
  }

  for (size_t i = 0; i < sizeof(DivCallName) / sizeof(DivCallName[0]); ++i) {
    const CallExpr *Call = Result.Nodes.getNodeAs<CallExpr>(DivCallName[i]);
    if (Call)
      checkCall(Result, *Call, DivCallN[i], DivCallN[i] - 1);
  }
}

void FloatArithPureCheck::checkCall(const ast_matchers::MatchFinder::MatchResult &Result, const CallExpr &Call, size_t N, size_t NonZeroParIdx) {
  assert(Result.Context);
  ASTContext &Context = *(Result.Context);

  char NameBuffer[5] = "par\0"; // ends w/ 2 0s
  size_t ConstCount = 0;
  for (size_t i = 0; i < N; ++i) {
    NameBuffer[3] = static_cast<char>('a' + i);
    const Expr *Parameter = Result.Nodes.getNodeAs<Expr>(NameBuffer);
    assert(Parameter);
    Optional<llvm::APSInt> OptVal = Parameter->getIntegerConstantExpr(Context);
    if (OptVal) {
      if ((i == NonZeroParIdx) && !*OptVal) {
	const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
	std::string FunctionName = Matched->getDeclName().getAsString();
	diag(Parameter->getBeginLoc(), "function %0 will fail division by 0") << FunctionName;
	return;
      }

      ++ConstCount;
    }
  }

  if (ConstCount == N) {
    const FunctionDecl *Matched = Result.Nodes.getNodeAs<FunctionDecl>("declaration");
    std::string FunctionName = Matched->getDeclName().getAsString();
    SourceLocation End;
    llvm::StringRef Name;

    if (const DeclStmt *DeclarationStatement = Result.Nodes.getNodeAs<DeclStmt>("declarationStatement")) {
      // the conditions look like they could be replaced by simply
      // starting the matcher with compoundStmt, but it doesn't always
      // work there...
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
      // see above
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

} // namespace hooks
} // namespace tidy
} // namespace clang
