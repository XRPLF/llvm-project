; RUN: llc -mtriple powerpc-ibm-aix-xcoff -verify-machineinstrs < %s | FileCheck %s
; RUN: not --crash llc -filetype=obj -mtriple powerpc-ibm-aix-xcoff  \
; RUN:                 -verify-machineinstrs < %s 2>&1 | \
; RUN:   FileCheck %s --check-prefix=OBJ32

;; FIXME: currently only fileHeader and sectionHeaders are supported in XCOFF64.

@i = external global i32, align 4  #0

; Function Attrs: noinline nounwind optnone
define i32* @get() {
  entry:
    ret i32* @i
}

; CHECK:        la 3, i[TD](2)
; CHECK:        .toc
; CHECK-NEXT:   .extern i[TD]

; OBJ32: LLVM ERROR:  toc-data not yet supported when writing object files.

attributes #0 = { "toc-data" }
