// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py
// RUN: %clang_cc1 -no-opaque-pointers -O1 -disable-llvm-passes -emit-llvm %s -o - -triple=x86_64-linux-gnu -verify
// RUN: %clang_cc1 -no-opaque-pointers -O1 -disable-llvm-passes -emit-llvm %s -o - -triple=x86_64-linux-gnu | FileCheck %s

// CHECK-LABEL: @_Z2wli(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 [[E:%.*]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2:![0-9]+]]
// CHECK-NEXT:    br label [[WHILE_COND:%.*]]
// CHECK:       while.cond:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i32 [[TMP0]], 0
// CHECK-NEXT:    [[TOBOOL_EXPVAL:%.*]] = call i1 @llvm.expect.i1(i1 [[TOBOOL]], i1 true)
// CHECK-NEXT:    br i1 [[TOBOOL_EXPVAL]], label [[WHILE_BODY:%.*]], label [[WHILE_END:%.*]]
// CHECK:       while.body:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP1]], 1
// CHECK-NEXT:    store i32 [[INC]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[WHILE_COND]], !llvm.loop [[LOOP6:![0-9]+]]
// CHECK:       while.end:
// CHECK-NEXT:    ret void
//
void wl(int e){
  while(e) [[likely]] ++e;
}

// CHECK-LABEL: @_Z2wui(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 [[E:%.*]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[WHILE_COND:%.*]]
// CHECK:       while.cond:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TOBOOL:%.*]] = icmp ne i32 [[TMP0]], 0
// CHECK-NEXT:    [[TOBOOL_EXPVAL:%.*]] = call i1 @llvm.expect.i1(i1 [[TOBOOL]], i1 false)
// CHECK-NEXT:    br i1 [[TOBOOL_EXPVAL]], label [[WHILE_BODY:%.*]], label [[WHILE_END:%.*]]
// CHECK:       while.body:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP1]], 1
// CHECK-NEXT:    store i32 [[INC]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[WHILE_COND]], !llvm.loop [[LOOP9:![0-9]+]]
// CHECK:       while.end:
// CHECK-NEXT:    ret void
//
void wu(int e){
  while(e) [[unlikely]] ++e;
}

// CHECK-LABEL: @_Z15w_branch_elidedj(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 [[E:%.*]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[WHILE_BODY:%.*]]
// CHECK:       while.body:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[INC:%.*]] = add i32 [[TMP0]], 1
// CHECK-NEXT:    store i32 [[INC]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[WHILE_BODY]], !llvm.loop [[LOOP10:![0-9]+]]
//
void w_branch_elided(unsigned e){
  // expected-warning@+2 {{attribute 'likely' has no effect when annotating an infinite loop}}
  // expected-note@+1 {{annotating the infinite loop here}}
  while(1) [[likely]] ++e;
}

// CHECK-LABEL: @_Z2flj(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 [[E:%.*]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TMP0:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 4, i8* [[TMP0]]) #[[ATTR4:[0-9]+]]
// CHECK-NEXT:    store i32 0, i32* [[I]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[FOR_COND:%.*]]
// CHECK:       for.cond:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, i32* [[I]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[CMP:%.*]] = icmp ne i32 [[TMP1]], [[TMP2]]
// CHECK-NEXT:    [[CMP_EXPVAL:%.*]] = call i1 @llvm.expect.i1(i1 [[CMP]], i1 true)
// CHECK-NEXT:    br i1 [[CMP_EXPVAL]], label [[FOR_BODY:%.*]], label [[FOR_COND_CLEANUP:%.*]]
// CHECK:       for.cond.cleanup:
// CHECK-NEXT:    [[TMP3:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 4, i8* [[TMP3]]) #[[ATTR4]]
// CHECK-NEXT:    br label [[FOR_END:%.*]]
// CHECK:       for.body:
// CHECK-NEXT:    br label [[FOR_INC:%.*]]
// CHECK:       for.inc:
// CHECK-NEXT:    [[TMP4:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[INC:%.*]] = add i32 [[TMP4]], 1
// CHECK-NEXT:    store i32 [[INC]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[FOR_COND]], !llvm.loop [[LOOP11:![0-9]+]]
// CHECK:       for.end:
// CHECK-NEXT:    ret void
//
void fl(unsigned e)
{
  for(int i = 0; i != e; ++e) [[likely]];
}

// CHECK-LABEL: @_Z2fui(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store i32 [[E:%.*]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TMP0:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 4, i8* [[TMP0]]) #[[ATTR4]]
// CHECK-NEXT:    store i32 0, i32* [[I]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[FOR_COND:%.*]]
// CHECK:       for.cond:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, i32* [[I]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[CMP:%.*]] = icmp ne i32 [[TMP1]], [[TMP2]]
// CHECK-NEXT:    [[CMP_EXPVAL:%.*]] = call i1 @llvm.expect.i1(i1 [[CMP]], i1 false)
// CHECK-NEXT:    br i1 [[CMP_EXPVAL]], label [[FOR_BODY:%.*]], label [[FOR_COND_CLEANUP:%.*]]
// CHECK:       for.cond.cleanup:
// CHECK-NEXT:    [[TMP3:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 4, i8* [[TMP3]]) #[[ATTR4]]
// CHECK-NEXT:    br label [[FOR_END:%.*]]
// CHECK:       for.body:
// CHECK-NEXT:    br label [[FOR_INC:%.*]]
// CHECK:       for.inc:
// CHECK-NEXT:    [[TMP4:%.*]] = load i32, i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[INC:%.*]] = add nsw i32 [[TMP4]], 1
// CHECK-NEXT:    store i32 [[INC]], i32* [[E_ADDR]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    br label [[FOR_COND]], !llvm.loop [[LOOP12:![0-9]+]]
// CHECK:       for.end:
// CHECK-NEXT:    ret void
//
void fu(int e)
{
  for(int i = 0; i != e; ++e) [[unlikely]];
}

// CHECK-LABEL: @_Z15f_branch_elidedv(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    br label [[FOR_COND:%.*]]
// CHECK:       for.cond:
// CHECK-NEXT:    br label [[FOR_COND]], !llvm.loop [[LOOP13:![0-9]+]]
//
void f_branch_elided()
{
  for(;;) [[likely]];
}

// CHECK-LABEL: @_Z3frlOA4_i(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca [4 x i32]*, align 8
// CHECK-NEXT:    [[__RANGE1:%.*]] = alloca [4 x i32]*, align 8
// CHECK-NEXT:    [[__BEGIN1:%.*]] = alloca i32*, align 8
// CHECK-NEXT:    [[__END1:%.*]] = alloca i32*, align 8
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store [4 x i32]* [[E:%.*]], [4 x i32]** [[E_ADDR]], align 8, !tbaa [[TBAA14:![0-9]+]]
// CHECK-NEXT:    [[TMP0:%.*]] = bitcast [4 x i32]** [[__RANGE1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 8, i8* [[TMP0]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP1:%.*]] = load [4 x i32]*, [4 x i32]** [[E_ADDR]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    store [4 x i32]* [[TMP1]], [4 x i32]** [[__RANGE1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP2:%.*]] = bitcast i32** [[__BEGIN1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 8, i8* [[TMP2]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP3:%.*]] = load [4 x i32]*, [4 x i32]** [[__RANGE1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[ARRAYDECAY:%.*]] = getelementptr inbounds [4 x i32], [4 x i32]* [[TMP3]], i64 0, i64 0
// CHECK-NEXT:    store i32* [[ARRAYDECAY]], i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP4:%.*]] = bitcast i32** [[__END1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 8, i8* [[TMP4]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP5:%.*]] = load [4 x i32]*, [4 x i32]** [[__RANGE1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[ARRAYDECAY1:%.*]] = getelementptr inbounds [4 x i32], [4 x i32]* [[TMP5]], i64 0, i64 0
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i32, i32* [[ARRAYDECAY1]], i64 4
// CHECK-NEXT:    store i32* [[ADD_PTR]], i32** [[__END1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    br label [[FOR_COND:%.*]]
// CHECK:       for.cond:
// CHECK-NEXT:    [[TMP6:%.*]] = load i32*, i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP7:%.*]] = load i32*, i32** [[__END1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[CMP:%.*]] = icmp ne i32* [[TMP6]], [[TMP7]]
// CHECK-NEXT:    [[CMP_EXPVAL:%.*]] = call i1 @llvm.expect.i1(i1 [[CMP]], i1 true)
// CHECK-NEXT:    br i1 [[CMP_EXPVAL]], label [[FOR_BODY:%.*]], label [[FOR_COND_CLEANUP:%.*]]
// CHECK:       for.cond.cleanup:
// CHECK-NEXT:    [[TMP8:%.*]] = bitcast i32** [[__END1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 8, i8* [[TMP8]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP9:%.*]] = bitcast i32** [[__BEGIN1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 8, i8* [[TMP9]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP10:%.*]] = bitcast [4 x i32]** [[__RANGE1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 8, i8* [[TMP10]]) #[[ATTR4]]
// CHECK-NEXT:    br label [[FOR_END:%.*]]
// CHECK:       for.body:
// CHECK-NEXT:    [[TMP11:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 4, i8* [[TMP11]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP12:%.*]] = load i32*, i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP13:%.*]] = load i32, i32* [[TMP12]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    store i32 [[TMP13]], i32* [[I]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TMP14:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 4, i8* [[TMP14]]) #[[ATTR4]]
// CHECK-NEXT:    br label [[FOR_INC:%.*]]
// CHECK:       for.inc:
// CHECK-NEXT:    [[TMP15:%.*]] = load i32*, i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[INCDEC_PTR:%.*]] = getelementptr inbounds i32, i32* [[TMP15]], i32 1
// CHECK-NEXT:    store i32* [[INCDEC_PTR]], i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    br label [[FOR_COND]], !llvm.loop [[LOOP16:![0-9]+]]
// CHECK:       for.end:
// CHECK-NEXT:    ret void
//
void frl(int (&&e) [4])
{
  for(int i : e) [[likely]];
}

// CHECK-LABEL: @_Z3fruOA4_i(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[E_ADDR:%.*]] = alloca [4 x i32]*, align 8
// CHECK-NEXT:    [[__RANGE1:%.*]] = alloca [4 x i32]*, align 8
// CHECK-NEXT:    [[__BEGIN1:%.*]] = alloca i32*, align 8
// CHECK-NEXT:    [[__END1:%.*]] = alloca i32*, align 8
// CHECK-NEXT:    [[I:%.*]] = alloca i32, align 4
// CHECK-NEXT:    store [4 x i32]* [[E:%.*]], [4 x i32]** [[E_ADDR]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP0:%.*]] = bitcast [4 x i32]** [[__RANGE1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 8, i8* [[TMP0]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP1:%.*]] = load [4 x i32]*, [4 x i32]** [[E_ADDR]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    store [4 x i32]* [[TMP1]], [4 x i32]** [[__RANGE1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP2:%.*]] = bitcast i32** [[__BEGIN1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 8, i8* [[TMP2]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP3:%.*]] = load [4 x i32]*, [4 x i32]** [[__RANGE1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[ARRAYDECAY:%.*]] = getelementptr inbounds [4 x i32], [4 x i32]* [[TMP3]], i64 0, i64 0
// CHECK-NEXT:    store i32* [[ARRAYDECAY]], i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP4:%.*]] = bitcast i32** [[__END1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 8, i8* [[TMP4]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP5:%.*]] = load [4 x i32]*, [4 x i32]** [[__RANGE1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[ARRAYDECAY1:%.*]] = getelementptr inbounds [4 x i32], [4 x i32]* [[TMP5]], i64 0, i64 0
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i32, i32* [[ARRAYDECAY1]], i64 4
// CHECK-NEXT:    store i32* [[ADD_PTR]], i32** [[__END1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    br label [[FOR_COND:%.*]]
// CHECK:       for.cond:
// CHECK-NEXT:    [[TMP6:%.*]] = load i32*, i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP7:%.*]] = load i32*, i32** [[__END1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[CMP:%.*]] = icmp ne i32* [[TMP6]], [[TMP7]]
// CHECK-NEXT:    [[CMP_EXPVAL:%.*]] = call i1 @llvm.expect.i1(i1 [[CMP]], i1 false)
// CHECK-NEXT:    br i1 [[CMP_EXPVAL]], label [[FOR_BODY:%.*]], label [[FOR_COND_CLEANUP:%.*]]
// CHECK:       for.cond.cleanup:
// CHECK-NEXT:    [[TMP8:%.*]] = bitcast i32** [[__END1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 8, i8* [[TMP8]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP9:%.*]] = bitcast i32** [[__BEGIN1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 8, i8* [[TMP9]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP10:%.*]] = bitcast [4 x i32]** [[__RANGE1]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 8, i8* [[TMP10]]) #[[ATTR4]]
// CHECK-NEXT:    br label [[FOR_END:%.*]]
// CHECK:       for.body:
// CHECK-NEXT:    [[TMP11:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.start.p0i8(i64 4, i8* [[TMP11]]) #[[ATTR4]]
// CHECK-NEXT:    [[TMP12:%.*]] = load i32*, i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[TMP13:%.*]] = load i32, i32* [[TMP12]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    store i32 [[TMP13]], i32* [[I]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[TMP14:%.*]] = bitcast i32* [[I]] to i8*
// CHECK-NEXT:    call void @llvm.lifetime.end.p0i8(i64 4, i8* [[TMP14]]) #[[ATTR4]]
// CHECK-NEXT:    br label [[FOR_INC:%.*]]
// CHECK:       for.inc:
// CHECK-NEXT:    [[TMP15:%.*]] = load i32*, i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    [[INCDEC_PTR:%.*]] = getelementptr inbounds i32, i32* [[TMP15]], i32 1
// CHECK-NEXT:    store i32* [[INCDEC_PTR]], i32** [[__BEGIN1]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    br label [[FOR_COND]], !llvm.loop [[LOOP17:![0-9]+]]
// CHECK:       for.end:
// CHECK-NEXT:    ret void
//
void fru(int (&&e) [4])
{
  for(int i : e) [[unlikely]];
}
