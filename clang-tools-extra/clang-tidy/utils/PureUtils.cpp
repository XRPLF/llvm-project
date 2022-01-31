//===--- PureUtils.cpp - clang-tidy----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "PureUtils.h"
#include "LexerUtils.h"
#include "clang/Basic/SourceManager.h"
#include <assert.h>

namespace clang {
namespace tidy {
namespace utils {
namespace pure {

using utils::lexer::getTokenKind;
using utils::lexer::forwardSkipWhitespaceAndComments;
  
static SourceLocation checkLocationForFix(SourceLocation TargetLoc, const ASTContext &Context, const SourceManager &SM, const char *TraceMethod) {
  if (!TargetLoc.isValid())
    return TargetLoc;

  SourceLocation EndLoc = Lexer::getLocForEndOfToken(TargetLoc, 0, SM, Context.getLangOpts());
  if (!EndLoc.isValid())
    return TargetLoc;

  const char *Begin = SM.getCharacterData(TargetLoc);
  const char *End = SM.getCharacterData(EndLoc);
  llvm::StringRef TestToken(Begin, End - Begin);
  if (!TestToken.compare(TraceMethod)) // fix apparently already applied
    return SourceLocation();

  return TargetLoc;
}

SourceLocation condSkipDeclaration(SourceLocation Loc, const ASTContext &Context, const char *TraceMethod) {
  if (!Loc.isValid())
    return Loc;

  SourceLocation NextLoc = Loc.getLocWithOffset(1);
  if (!NextLoc.isValid())
    return NextLoc;

  const SourceManager &SM = Context.getSourceManager();
  SourceLocation EndLoc = forwardSkipWhitespaceAndComments(NextLoc, SM, &Context);
  return checkLocationForFix(EndLoc, Context, SM, TraceMethod);
}

SourceLocation condSkipStatement(SourceLocation Loc, const ASTContext &Context, const char *TraceMethod) {
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
  if (TokKind != tok::semi) // happens for if statements
    return checkLocationForFix(SemiLoc, Context, SM, TraceMethod);

  SourceLocation AfterLoc = Lexer::getLocForEndOfToken(SemiLoc, 0, SM, Context.getLangOpts());
  if (!AfterLoc.isValid())
    return InvalidLoc;

  SourceLocation EndLoc = AfterLoc.getLocWithOffset(1);
  return checkLocationForFix(EndLoc, Context, SM, TraceMethod);
}

std::string makeTraceFix(llvm::StringRef BufferName, int HexFlag) {
  assert ((0 <= HexFlag) && (HexFlag <= 1));

  std::string Fix("trace((uint32_t)\"");
  Fix += BufferName;
  Fix += "\", sizeof(\"";
  Fix += BufferName;
  Fix += "\"), (uint32_t)";
  Fix += BufferName;
  Fix += ", sizeof(";
  Fix += BufferName;
  Fix += "), ";
  Fix += std::to_string(HexFlag);
  Fix += ");\n";
  return Fix;
}

std::string makeTraceFloatFix(llvm::StringRef BufferName) {
  std::string Fix("trace_num((uint32_t)\"");
  Fix += BufferName;
  Fix += "\", sizeof(\"";
  Fix += BufferName;
  Fix += "\"), (int64_t)";
  Fix += BufferName;
  Fix += ");\n";
  return Fix;
}

} // namespace pure
} // namespace utils
} // namespace tidy
} // namespace clang
  
