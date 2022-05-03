//===--- ReleaseDefineCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ReleaseDefineCheck.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

namespace {
  
class ReleaseDefineScan : public PPCallbacks {
public:
  ReleaseDefineScan(ClangTidyCheck &Check, Preprocessor &PP)
    : Check(Check), PP(PP) {}

  void MacroExpands(const Token &MacroNameTok, const MacroDefinition &MD,
                    SourceRange Range, const MacroArgs *Args) override;
private:
  ClangTidyCheck &Check;
  Preprocessor &PP;
};

void ReleaseDefineScan::MacroExpands(const Token &MacroNameTok, const MacroDefinition &MD,
                                     SourceRange Range, const MacroArgs *Args) {
  if (!PP.isMacroDefined("NDEBUG"))
    return;

  IdentifierInfo *Info = MacroNameTok.getIdentifierInfo();
  llvm::StringRef Name(Info->getNameStart());
  if (!(Name.equals("TRACEVAR") || Name.equals("TRACEHEX") || Name.equals("TRACEXFL") || Name.equals("TRACESTR")))
    return;

  IdentifierTable &Table = PP.getIdentifierTable();
  IdentifierTable::iterator it = Table.find("DEBUG");
  // can happen if user defines NDEBUG without including anything, but
  // in that case they'll have to define the tracing macro as well...
  if (it == Table.end())
    return;
  
  MacroInfo *DebugInfo = PP.getMacroInfo(it->second);
  assert(DebugInfo);

  // can happen if user defines their own DEBUG (which, again, will
  // probably break the tracing macro)
  if (DebugInfo->getNumTokens() != 1)
    return;

  MacroInfo::const_tokens_iterator tp = DebugInfo->tokens_begin();
  if (!tp->isLiteral())
    return;

  std::string Lit = PP.getSpelling(*tp);
  if (Lit != "0")
    Check.diag(MacroNameTok.getLocation(), "NDEBUG ignored when defined after including hook-specific headers");
}

} // anonymous namespace

void ReleaseDefineCheck::registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                                             Preprocessor *ModuleExpanderPP) {
  PP->addPPCallbacks(std::make_unique<ReleaseDefineScan>(*this, *PP));
}

} // namespace hooks
} // namespace tidy
} // namespace clang
