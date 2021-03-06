; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -passes=constraint-elimination -S %s | FileCheck %s

%struct.1 = type { i32, i64, i8 }

define i1 @test.ult.true.due.to.first.dimension(%struct.1* %start, i8* %high) {
; CHECK-LABEL: @test.ult.true.due.to.first.dimension(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 6, i32 0
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i32* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 5, i32 0
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i32* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 6, i32 0
  %add.ptr.cast = bitcast i32* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 5, i32 0
  %start.0.cast = bitcast i32* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.true.due.to.first.dimension.var.index.0(%struct.1* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @test.ult.true.due.to.first.dimension.var.index.0(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT_PLUS_1:%.*]] = add nuw nsw i32 [[IDX:%.*]], 1
; CHECK-NEXT:    [[IDX_EXT_PLUS_1_EXT:%.*]] = zext i32 [[IDX_EXT_PLUS_1]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 [[IDX_EXT_PLUS_1_EXT]], i32 0
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i32* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX]] to i64
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 [[IDX_EXT]], i32 0
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i32* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext.plus.1 = add nuw nsw i32 %idx, 1
  %idx.ext.plus.1.ext = zext i32 %idx.ext.plus.1 to i64
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext.plus.1.ext, i32 0
  %add.ptr.cast = bitcast i32* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %idx.ext = zext i32 %idx to i64
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 0
  %start.0.cast = bitcast i32* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.true.due.to.first.dimension.var.index.1(%struct.1* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @test.ult.true.due.to.first.dimension.var.index.1(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT_PLUS_1:%.*]] = add nuw nsw i32 [[IDX:%.*]], 1
; CHECK-NEXT:    [[IDX_EXT_PLUS_1_EXT:%.*]] = zext i32 [[IDX_EXT_PLUS_1]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 [[IDX_EXT_PLUS_1_EXT]], i32 0
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i32* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX]] to i64
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 [[IDX_EXT]], i32 1
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i64* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext.plus.1 = add nuw nsw i32 %idx, 1
  %idx.ext.plus.1.ext = zext i32 %idx.ext.plus.1 to i64
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext.plus.1.ext, i32 0
  %add.ptr.cast = bitcast i32* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %idx.ext = zext i32 %idx to i64
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 1
  %start.0.cast = bitcast i64* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.true.due.to.first.dimension.var.index.2(%struct.1* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @test.ult.true.due.to.first.dimension.var.index.2(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT_PLUS_1:%.*]] = add nuw nsw i32 [[IDX:%.*]], 1
; CHECK-NEXT:    [[IDX_EXT_PLUS_1_EXT:%.*]] = zext i32 [[IDX_EXT_PLUS_1]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 [[IDX_EXT_PLUS_1_EXT]], i32 0
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i32* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX]] to i64
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 [[IDX_EXT]], i32 2
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext.plus.1 = add nuw nsw i32 %idx, 1
  %idx.ext.plus.1.ext = zext i32 %idx.ext.plus.1 to i64
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext.plus.1.ext, i32 0
  %add.ptr.cast = bitcast i32* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %idx.ext = zext i32 %idx to i64
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 2
  %c.0 = icmp ult i8* %start.0, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.unknown.due.to.first.dimension(%struct.1* %start, i8* %high) {
; CHECK-LABEL: @test.ult.unknown.due.to.first.dimension(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 5, i32 0
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i32* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 6, i32 0
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i32* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 5, i32 0
  %add.ptr.cast = bitcast i32* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 6, i32 0
  %start.0.cast = bitcast i32* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.true.due.to.second.dimension(%struct.1* %start, i8* %high) {
; CHECK-LABEL: @test.ult.true.due.to.second.dimension(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 5, i32 2
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 5, i32 1
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i64* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 5, i32 2
  %c.1 = icmp ule i8* %add.ptr, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 5, i32 1
  %start.0.cast = bitcast i64 * %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.unknown.due.to.second.dimension(%struct.1* %start, i8* %high) {
; CHECK-LABEL: @test.ult.unknown.due.to.second.dimension(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 5, i32 2
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 5, i32 2
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 5, i32 2
  %c.1 = icmp ule i8* %add.ptr, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 5, i32 2
  %c.0 = icmp ult i8* %start.0, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.unknown.due.to.second.dimension.var.index.0(%struct.1* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @test.ult.unknown.due.to.second.dimension.var.index.0(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX:%.*]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 [[IDX_EXT]], i32 0
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i32* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 [[IDX_EXT]], i32 1
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i64* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext = zext i32 %idx to i64
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 0
  %add.ptr.cast = bitcast i32* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 1
  %start.0.cast = bitcast i64* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @test.ult.unknown.due.to.second.dimension.var.index.1(%struct.1* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @test.ult.unknown.due.to.second.dimension.var.index.1(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX:%.*]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_1:%.*]], %struct.1* [[START:%.*]], i64 [[IDX_EXT]], i32 1
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i64* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_1]], %struct.1* [[START]], i64 [[IDX_EXT]], i32 2
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext = zext i32 %idx to i64
  %add.ptr = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 1
  %add.ptr.cast = bitcast i64* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.1, %struct.1* %start, i64 %idx.ext, i32 2
  %c.0 = icmp ult i8* %start.0, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

%struct.2 = type { i32, [20 x i64], i8 }

define i1 @ptr.int.struct.test.ult.true.due.to.first.dimension(%struct.2* %start, i8* %high) {
; CHECK-LABEL: @ptr.int.struct.test.ult.true.due.to.first.dimension(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_2:%.*]], %struct.2* [[START:%.*]], i64 6, i32 1, i32 5
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i64* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_2]], %struct.2* [[START]], i64 6, i32 0
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i32* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %add.ptr = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 1, i32 5
  %add.ptr.cast = bitcast i64* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %start.0 = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 0
  %start.0.cast = bitcast i32* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @ptr.int.struct.test.ult.true.due.to.third.dimension.var.index(%struct.2* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @ptr.int.struct.test.ult.true.due.to.third.dimension.var.index(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT_PLUS_1:%.*]] = add nuw nsw i32 [[IDX:%.*]], 1
; CHECK-NEXT:    [[IDX_EXT_PLUS_1_EXT:%.*]] = zext i32 [[IDX_EXT_PLUS_1]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_2:%.*]], %struct.2* [[START:%.*]], i64 6, i32 1, i64 [[IDX_EXT_PLUS_1_EXT]]
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i64* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX]] to i64
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_2]], %struct.2* [[START]], i64 6, i32 1, i64 [[IDX_EXT]]
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i64* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext.plus.1 = add nuw nsw i32 %idx, 1
  %idx.ext.plus.1.ext = zext i32 %idx.ext.plus.1 to i64
  %add.ptr = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 1, i64 %idx.ext.plus.1.ext
  %add.ptr.cast = bitcast i64* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %idx.ext = zext i32 %idx to i64
  %start.0 = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 1, i64 %idx.ext
  %start.0.cast = bitcast i64* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @ptr.int.struct.test.ult.due.to.second.dimension.var.index(%struct.2* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @ptr.int.struct.test.ult.due.to.second.dimension.var.index(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT_PLUS_1:%.*]] = add nuw nsw i32 [[IDX:%.*]], 1
; CHECK-NEXT:    [[IDX_EXT_PLUS_1_EXT:%.*]] = zext i32 [[IDX_EXT_PLUS_1]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_2:%.*]], %struct.2* [[START:%.*]], i64 6, i32 1, i64 [[IDX_EXT_PLUS_1_EXT]]
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i64* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX]] to i64
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_2]], %struct.2* [[START]], i64 6, i32 0
; CHECK-NEXT:    [[START_0_CAST:%.*]] = bitcast i32* [[START_0]] to i8*
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0_CAST]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext.plus.1 = add nuw nsw i32 %idx, 1
  %idx.ext.plus.1.ext = zext i32 %idx.ext.plus.1 to i64
  %add.ptr = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 1, i64 %idx.ext.plus.1.ext
  %add.ptr.cast = bitcast i64* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %idx.ext = zext i32 %idx to i64
  %start.0 = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 0
  %start.0.cast = bitcast i32* %start.0 to i8*
  %c.0 = icmp ult i8* %start.0.cast, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}

define i1 @ptr.int.struct.test.ult.unknown.due.to.second.dimension.var.index(%struct.2* %start, i8* %high, i32 %idx) {
; CHECK-LABEL: @ptr.int.struct.test.ult.unknown.due.to.second.dimension.var.index(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[IDX_EXT_PLUS_1:%.*]] = add nuw nsw i32 [[IDX:%.*]], 1
; CHECK-NEXT:    [[IDX_EXT_PLUS_1_EXT:%.*]] = zext i32 [[IDX_EXT_PLUS_1]] to i64
; CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds [[STRUCT_2:%.*]], %struct.2* [[START:%.*]], i64 6, i32 1, i64 [[IDX_EXT_PLUS_1_EXT]]
; CHECK-NEXT:    [[ADD_PTR_CAST:%.*]] = bitcast i64* [[ADD_PTR]] to i8*
; CHECK-NEXT:    [[C_1:%.*]] = icmp ule i8* [[ADD_PTR_CAST]], [[HIGH:%.*]]
; CHECK-NEXT:    br i1 [[C_1]], label [[IF_THEN:%.*]], label [[IF_END:%.*]]
; CHECK:       if.then:
; CHECK-NEXT:    [[IDX_EXT:%.*]] = zext i32 [[IDX]] to i64
; CHECK-NEXT:    [[START_0:%.*]] = getelementptr inbounds [[STRUCT_2]], %struct.2* [[START]], i64 6, i32 2
; CHECK-NEXT:    [[C_0:%.*]] = icmp ult i8* [[START_0]], [[HIGH]]
; CHECK-NEXT:    ret i1 [[C_0]]
; CHECK:       if.end:
; CHECK-NEXT:    ret i1 true
;
entry:
  %idx.ext.plus.1 = add nuw nsw i32 %idx, 1
  %idx.ext.plus.1.ext = zext i32 %idx.ext.plus.1 to i64
  %add.ptr = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 1, i64 %idx.ext.plus.1.ext
  %add.ptr.cast = bitcast i64* %add.ptr to i8*
  %c.1 = icmp ule i8* %add.ptr.cast, %high
  br i1 %c.1, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %idx.ext = zext i32 %idx to i64
  %start.0 = getelementptr inbounds %struct.2, %struct.2* %start, i64 6, i32 2
  %c.0 = icmp ult i8* %start.0, %high
  ret i1 %c.0

if.end:                                           ; preds = %entry
  ret i1 1
}
