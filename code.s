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
; Main
; -------------------
Main:
        LDI Const0
        STA Counter1
        STA Counter2
        STA Counter3
        STA Counter4
        STA TaskIndex

        B Task1         ; start with Task1

; -------------------
; Yield: save A/X on entry, restore before every exit
; No JSR/RTS used anywhere -> pushes cannot be mistaken as return addresses.
; -------------------
Yield:
        ; Save caller-scratch so we can freely use A/X here
        PHA
        PHX

        ; index++
        LDA TaskIndex
        INC
        STA TaskIndex

        ; A := TaskIndex for all subsequent SUBF compares
        LDA TaskIndex

        ; wrap if index == 4
        LDX Const4
        SUBF
        BZ Y_Wrap

        ; index == 0 ?
        LDX Const0
        SUBF
        BZ Y_Go1

        ; index == 1 ?
        LDX Const1
        SUBF
        BZ Y_Go2

        ; index == 2 ?
        LDX Const2
        SUBF
        BZ Y_Go3

        ; else -> 3
        PLX
        PLA
        B Task4

Y_Wrap:
        LDI Const0
        STA TaskIndex
        ; after wrap, next is Task1
        PLX
        PLA
        B Task1

Y_Go1:
        PLX
        PLA
        B Task1

Y_Go2:
        PLX
        PLA
        B Task2

Y_Go3:
        PLX
        PLA
        B Task3

; -------------------
; Tasks: do work then B to Yield (no stack involvement)
; -------------------
Task1:
        LDA Counter1
        INC
        STA Counter1
        B Yield

Task2:
        LDA Counter2
        INC
        STA Counter2
        B Yield

Task3:
        LDA Counter3
        INC
        STA Counter3
        B Yield

Task4:
        LDA Counter4
        INC
        STA Counter4
        B Yield

