; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; Test some floating point casting cases
; RUN: opt < %s -passes=instcombine -S | FileCheck %s

define i8 @test1() {
; CHECK-LABEL: @test1(
; CHECK-NEXT:    ret i8 -1
;
  %x = fptoui float 2.550000e+02 to i8
  ret i8 %x
}

define i8 @test2() {
; CHECK-LABEL: @test2(
; CHECK-NEXT:    ret i8 -1
;
  %x = fptosi float -1.000000e+00 to i8
  ret i8 %x
}

define half @test3(float %a) {
; CHECK-LABEL: @test3(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc float [[A:%.*]] to half
; CHECK-NEXT:    [[C:%.*]] = call half @llvm.fabs.f16(half [[TMP1]])
; CHECK-NEXT:    ret half [[C]]
;
  %b = call float @llvm.fabs.f32(float %a)
  %c = fptrunc float %b to half
  ret half %c
}

define half @fneg_fptrunc(float %a) {
; CHECK-LABEL: @fneg_fptrunc(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc float [[A:%.*]] to half
; CHECK-NEXT:    [[C:%.*]] = fneg half [[TMP1]]
; CHECK-NEXT:    ret half [[C]]
;
  %b = fsub float -0.0, %a
  %c = fptrunc float %b to half
  ret half %c
}

define half @unary_fneg_fptrunc(float %a) {
; CHECK-LABEL: @unary_fneg_fptrunc(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc float [[A:%.*]] to half
; CHECK-NEXT:    [[C:%.*]] = fneg half [[TMP1]]
; CHECK-NEXT:    ret half [[C]]
;
  %b = fneg float %a
  %c = fptrunc float %b to half
  ret half %c
}

define <2 x half> @fneg_fptrunc_vec_undef(<2 x float> %a) {
; CHECK-LABEL: @fneg_fptrunc_vec_undef(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc <2 x float> [[A:%.*]] to <2 x half>
; CHECK-NEXT:    [[C:%.*]] = fneg <2 x half> [[TMP1]]
; CHECK-NEXT:    ret <2 x half> [[C]]
;
  %b = fsub <2 x float> <float -0.0, float undef>, %a
  %c = fptrunc <2 x float> %b to <2 x half>
  ret <2 x half> %c
}

define <2 x half> @unary_fneg_fptrunc_vec(<2 x float> %a) {
; CHECK-LABEL: @unary_fneg_fptrunc_vec(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc <2 x float> [[A:%.*]] to <2 x half>
; CHECK-NEXT:    [[C:%.*]] = fneg <2 x half> [[TMP1]]
; CHECK-NEXT:    ret <2 x half> [[C]]
;
  %b = fneg <2 x float> %a
  %c = fptrunc <2 x float> %b to <2 x half>
  ret <2 x half> %c
}

define half @test4-fast(float %a) {
; CHECK-LABEL: @test4-fast(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc float [[A:%.*]] to half
; CHECK-NEXT:    [[C:%.*]] = fneg fast half [[TMP1]]
; CHECK-NEXT:    ret half [[C]]
;
  %b = fsub fast float -0.0, %a
  %c = fptrunc float %b to half
  ret half %c
}

define half @test4_unary_fneg-fast(float %a) {
; CHECK-LABEL: @test4_unary_fneg-fast(
; CHECK-NEXT:    [[TMP1:%.*]] = fptrunc float [[A:%.*]] to half
; CHECK-NEXT:    [[C:%.*]] = fneg fast half [[TMP1]]
; CHECK-NEXT:    ret half [[C]]
;
  %b = fneg fast float %a
  %c = fptrunc float %b to half
  ret half %c
}

define half @test5(float %a, float %b, float %c) {
; CHECK-LABEL: @test5(
; CHECK-NEXT:    [[D:%.*]] = fcmp ogt float [[A:%.*]], [[B:%.*]]
; CHECK-NEXT:    [[E:%.*]] = select i1 [[D]], float [[C:%.*]], float 1.000000e+00
; CHECK-NEXT:    [[F:%.*]] = fptrunc float [[E]] to half
; CHECK-NEXT:    ret half [[F]]
;
  %d = fcmp ogt float %a, %b
  %e = select i1 %d, float %c, float 1.0
  %f = fptrunc float %e to half
  ret half %f
}

declare float @llvm.fabs.f32(float) nounwind readonly

define <1 x float> @test6(<1 x double> %V) {
; CHECK-LABEL: @test6(
; CHECK-NEXT:    [[FREM:%.*]] = frem <1 x double> [[V:%.*]], [[V]]
; CHECK-NEXT:    [[TRUNC:%.*]] = fptrunc <1 x double> [[FREM]] to <1 x float>
; CHECK-NEXT:    ret <1 x float> [[TRUNC]]
;
  %frem = frem <1 x double> %V, %V
  %trunc = fptrunc <1 x double> %frem to <1 x float>
  ret <1 x float> %trunc
}

define float @test7(double %V) {
; CHECK-LABEL: @test7(
; CHECK-NEXT:    [[FREM:%.*]] = frem double [[V:%.*]], 1.000000e+00
; CHECK-NEXT:    [[TRUNC:%.*]] = fptrunc double [[FREM]] to float
; CHECK-NEXT:    ret float [[TRUNC]]
;
  %frem = frem double %V, 1.000000e+00
  %trunc = fptrunc double %frem to float
  ret float %trunc
}

define float @test8(float %V) {
; CHECK-LABEL: @test8(
; CHECK-NEXT:    [[FEXT:%.*]] = fpext float [[V:%.*]] to double
; CHECK-NEXT:    [[FREM:%.*]] = frem double [[FEXT]], 1.000000e-01
; CHECK-NEXT:    [[TRUNC:%.*]] = fptrunc double [[FREM]] to float
; CHECK-NEXT:    ret float [[TRUNC]]
;
  %fext = fpext float %V to double
  %frem = frem double %fext, 1.000000e-01
  %trunc = fptrunc double %frem to float
  ret float %trunc
}

define half @test_fptrunc_fptrunc(double %V) {
; CHECK-LABEL: @test_fptrunc_fptrunc(
; CHECK-NEXT:    [[T1:%.*]] = fptrunc double [[V:%.*]] to float
; CHECK-NEXT:    [[T2:%.*]] = fptrunc float [[T1]] to half
; CHECK-NEXT:    ret half [[T2]]
;
  %t1 = fptrunc double %V to float
  %t2 = fptrunc float %t1 to half
  ret half %t2
}

