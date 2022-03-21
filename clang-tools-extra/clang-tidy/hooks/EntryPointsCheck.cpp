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

const char *EntryPointsCheck::Names[] = { "cbak", "hook" };

const char *EntryPointsCheck::DefaultFunctions[] = {
   "int64_t cbak(uint32_t what) { return 0; }\n",
   "int64_t hook(uint32_t reserved) { }\n"
};

void EntryPointsCheck::registerMatchers(MatchFinder *Finder) {
  for (size_t i = 0; i < sizeof(Names) / sizeof(Names[0]); ++i) {
    const char *Name = Names[i];
    Finder->addMatcher(translationUnitDecl(unless(hasDescendant(functionDecl(isDefinition(), hasName(Name))))).bind(getMatchName(Name)), this);
  }
}

void EntryPointsCheck::check(const MatchFinder::MatchResult &Result) {
  for (size_t i = 0; i < sizeof(Names) / sizeof(Names[0]); ++i) {
    const char *Name = Names[i];
    const auto *Matched = Result.Nodes.getNodeAs<TranslationUnitDecl>(getMatchName(Name));
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
	diag(End, "missing function '%0'", DiagnosticIDs::Error) << Name << FixItHint::CreateInsertion(End, DefaultFunctions[i]);
      }
    }
  }
}

std::string EntryPointsCheck::getMatchName(const char *Name) const {
  assert(Name);
  std::string MatchName("~");
  MatchName += Name;
  return MatchName;
}

} // namespace hooks
} // namespace tidy
} // namespace clang
