//===--- PureUtils.h - clang-tidy--------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_UTILS_PURE_UTILS_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_UTILS_PURE_UTILS_H

#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceLocation.h"
#include <string>

namespace clang {
namespace tidy {
namespace utils {
namespace pure {

SourceLocation condSkipDeclaration(SourceLocation Loc, const ASTContext &Context, const char *TraceMethod="trace");

SourceLocation condSkipStatement(SourceLocation Loc, const ASTContext &Context, const char *TraceMethod="trace");

std::string makeTraceFix(llvm::StringRef BufferName, int HexFlag);

std::string makeTraceFloatFix(llvm::StringRef BufferName);

} // namespace pure
} // namespace utils
} // namespace tidy
} // namespace clang
  
#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_UTILS_PURE_UTILS_H
