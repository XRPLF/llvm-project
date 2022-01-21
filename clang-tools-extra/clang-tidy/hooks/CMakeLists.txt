set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangTidyHooksModule
  EntryPointRecursionCheck.cpp
  EntryPointsCheck.cpp
  EntryPointsNegCheck.cpp
  GuardCalledCheck.cpp
  GuardInForCheck.cpp
  HooksTidyModule.cpp
  RaddrConvBufLenCheck.cpp
  RaddrConvPureCheck.cpp
  ReserveLimitCheck.cpp

  LINK_LIBS
  clangTidy
  clangTidyUtils

  DEPENDS
  omp_gen
  )

clang_target_link_libraries(clangTidyHooksModule
  PRIVATE
  clangAnalysis
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangSerialization
  clangTooling
  )
