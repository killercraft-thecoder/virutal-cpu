        .org 0

; -------------------
; Constants / Memory Map
; -------------------
.equ TaskIndex    $1000   ; current task index (0..3)
.equ Counter1     $1001
.equ Counter2     $1002
.equ Counter3     $1003
.equ Counter4     $1004

.equ Const0 0
.equ Const1 1
.equ Const2 2
.equ Const4 4

; -------------------
; Main init
; -------------------
Main:
        LDI Const0
        STA Counter1
        STA Counter2
        STA Counter3
        STA Counter4
        STA TaskIndex

        BR Task1         ; start with Task1


; -------------------
; Yield routine
; -------------------
Yield:
        ; Increment index
        LDA TaskIndex
        INC
        STA TaskIndex

        ; Wrap if index == 4
        LDA TaskIndex
        LDX Const4
        SUBF
        BZ WrapToZero
        BR CheckTask0

WrapToZero:
        LDI Const0
        STA TaskIndex

CheckTask0:
        ; index == 0 ?
        LDA TaskIndex
        LDX Const0
        SUBF
        BZ GoTask1

        ; index == 1 ?
        LDA TaskIndex
        LDX Const1
        SUBF
        BZ GoTask2

        ; index == 2 ?
        LDA TaskIndex
        LDX Const2
        SUBF
        BZ GoTask3

        ; else must be 3
        BR GoTask4


; -------------------
; Dispatch labels
; -------------------
GoTask1: BR Task1
GoTask2: BR Task2
GoTask3: BR Task3
GoTask4: BR Task4


; -------------------
; Tasks
; -------------------
Task1:
        LDA Counter1
        INC
        STA Counter1
        BR Yield

Task2:
        LDA Counter2
        INC
        STA Counter2
        BR Yield

Task3:
        LDA Counter3
        INC
        STA Counter3
        BR Yield

Task4:
        LDA Counter4
        INC
        STA Counter4
        BR Yield