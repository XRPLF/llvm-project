//===--- EntryPointsCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EntryPointsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <assert.h>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace hooks {

void EntryPointsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(translationUnitDecl(unless(hasDescendant(functionDecl(isDefinition(), hasName("hook"))))).bind("~hook"), this);
}

void EntryPointsCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *Matched = Result.Nodes.getNodeAs<TranslationUnitDecl>("~hook");
  if (Matched) {
    // TranslationUnitDecl has a location retrieval interface, but
    // the location is usually invalid, while the call to diag must
    // have a valid location for clangd to show the diagnostics.
    ASTContext &Context = Matched->getASTContext();
    SourceManager &Manager = Context.getSourceManager();
    FileID MainFileID = Manager.getMainFileID();

    // definition can be missing only in a source file (not header)
    const FileEntry *Entry = Manager.getFileEntryForID(MainFileID);
    llvm::StringRef MainFileName = Entry->getName();
    if (MainFileName.endswith_insensitive(".c")) {
      SourceLocation End = Manager.getLocForEndOfFile(MainFileID);
      diag(End, "missing function 'hook'", DiagnosticIDs::Error) << FixItHint::CreateInsertion(End, "int64_t hook(uint32_t ctx) { }\n");
    }
  }
}

} // namespace hooks
} // namespace tidy
} // namespace clang
