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
#include "AccountBufLenCheck.h"
#include "AccountConvBufLenCheck.h"
#include "AccountConvPureCheck.h"
#include "ArrayBufLenCheck.h"
#include "BurdenPrereqCheck.h"
#include "DetailBufLenCheck.h"
#include "DetailPrereqCheck.h"
#include "EmitBufLenCheck.h"
#include "EmitPrereqCheck.h"
#include "EntryPointRecursionCheck.h"
#include "EntryPointsCheck.h"
#include "EntryPointsNegCheck.h"
#include "FeePrereqCheck.h"
#include "FieldAddBufLenCheck.h"
#include "FieldBufLenCheck.h"
#include "FieldDelBufLenCheck.h"
#include "FloatArithPureCheck.h"
#include "FloatOnePureCheck.h"
#include "FloatPureCheck.h"
#include "GuardCalledCheck.h"
#include "GuardInForCheck.h"
#include "GuardInWhileCheck.h"
#include "HashBufLenCheck.h"
#include "KeyletBufLenCheck.h"
#include "RaddrConvBufLenCheck.h"
#include "RaddrConvPureCheck.h"
#include "ReserveLimitCheck.h"
#include "ValidateBufLenCheck.h"
#include "VerifyBufLenCheck.h"

namespace clang {
namespace tidy {
namespace hooks {

class HooksModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<AccountBufLenCheck>(
        "hooks-account-buf-len");
    CheckFactories.registerCheck<AccountConvBufLenCheck>(
        "hooks-account-conv-buf-len");
    CheckFactories.registerCheck<AccountConvPureCheck>(
        "hooks-account-conv-pure");
    CheckFactories.registerCheck<ArrayBufLenCheck>(
        "hooks-array-buf-len");
    CheckFactories.registerCheck<BurdenPrereqCheck>(
        "hooks-burden-prereq");
    CheckFactories.registerCheck<DetailBufLenCheck>(
        "hooks-detail-buf-len");
    CheckFactories.registerCheck<DetailPrereqCheck>(
        "hooks-detail-prereq");
    CheckFactories.registerCheck<EmitBufLenCheck>(
        "hooks-emit-buf-len");
    CheckFactories.registerCheck<EmitPrereqCheck>(
        "hooks-emit-prereq");
    CheckFactories.registerCheck<EntryPointRecursionCheck>(
        "hooks-entry-point-recursion");
    CheckFactories.registerCheck<EntryPointsCheck>(
        "hooks-entry-points");
    CheckFactories.registerCheck<EntryPointsNegCheck>(
	"hooks-entry-points-neg");
    CheckFactories.registerCheck<FeePrereqCheck>(
        "hooks-fee-prereq");
    CheckFactories.registerCheck<FieldAddBufLenCheck>(
        "hooks-field-add-buf-len");
    CheckFactories.registerCheck<FieldBufLenCheck>(
        "hooks-field-buf-len");
    CheckFactories.registerCheck<FieldDelBufLenCheck>(
        "hooks-field-del-buf-len");
    CheckFactories.registerCheck<FloatArithPureCheck>(
        "hooks-float-arith-pure");
    CheckFactories.registerCheck<FloatOnePureCheck>(
        "hooks-float-one-pure");
    CheckFactories.registerCheck<FloatPureCheck>(
        "hooks-float-pure");
    CheckFactories.registerCheck<GuardCalledCheck>(
	"hooks-guard-called");
    CheckFactories.registerCheck<GuardInForCheck>(
	"hooks-guard-in-for");
    CheckFactories.registerCheck<GuardInWhileCheck>(
        "hooks-guard-in-while");
    CheckFactories.registerCheck<HashBufLenCheck>(
        "hooks-hash-buf-len");
    CheckFactories.registerCheck<KeyletBufLenCheck>(
        "hooks-keylet-buf-len");
    CheckFactories.registerCheck<RaddrConvBufLenCheck>(
        "hooks-raddr-conv-buf-len");
    CheckFactories.registerCheck<RaddrConvPureCheck>(
        "hooks-raddr-conv-pure");
    CheckFactories.registerCheck<ReserveLimitCheck>(
        "hooks-reserve-limit");
    CheckFactories.registerCheck<ValidateBufLenCheck>(
        "hooks-validate-buf-len");
    CheckFactories.registerCheck<VerifyBufLenCheck>(
        "hooks-verify-buf-len");
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
