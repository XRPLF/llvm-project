//===--- HooksTidyModule.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "../ClangTidyModuleRegistry.h"
#include "HookEntryPointsCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

class HooksModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<HookEntryPointsCheck>(
	"hooks-entry-points");
  }
};

} // namespace hooks

// Register the HooksTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<hooks::HooksModule>
    X("hooks-module", "Checks specific to XRPLD hooks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the HooksModule.
volatile int HooksModuleAnchorSource = 0;

} // namespace tidy
} // namespace clang
