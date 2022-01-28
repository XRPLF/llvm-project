set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_library(clangTidyHooksModule
  AccountBufLenCheck.cpp
  AccountConvBufLenCheck.cpp
  AccountConvPureCheck.cpp
  ArrayBufLenCheck.cpp
  BurdenPrereqCheck.cpp
  EntryPointRecursionCheck.cpp
  EntryPointsCheck.cpp
  EntryPointsNegCheck.cpp
  FieldAddBufLenCheck.cpp
  FieldBufLenCheck.cpp
  FieldDelBufLenCheck.cpp
  GuardCalledCheck.cpp
  GuardInForCheck.cpp
  GuardInWhileCheck.cpp
  HashBufLenCheck.cpp
  HooksTidyModule.cpp
  KeyletBufLenCheck.cpp
  RaddrConvBufLenCheck.cpp
  RaddrConvPureCheck.cpp
  ReserveLimitCheck.cpp
  ValidateBufLenCheck.cpp
  VerifyBufLenCheck.cpp

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
