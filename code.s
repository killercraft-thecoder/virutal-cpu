        .org 0

; -------------------
; Constants / Memory Map
; -------------------

.equ TaskIndex   $1000   ; current task index (0..3)
.equ Counter1    $1001   ; input/output for Task1
.equ Counter2    $1002   ; input/output for Task2
.equ Counter3    $1003   ; input/output for Task3
.equ Counter4    $1004   ; input/output for Task4

.equ TempN1      $1010   ; Fib accumulator 1
.equ TempN2      $1011   ; Fib accumulator 2
.equ TempI,      $1012   ; Fib loop counter

.equ Const0     0
.equ Const1     1
.equ Const2      2
.equ Const4      4



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
; Task1: run Fib on Counter1
; -------------------
Task1:
        LDA Counter1
        STA TempI
        B FibRoutine

; -------------------
; Task2: tweak Counter2, then run Fib
; -------------------
Task2:
        LDA Counter2
        INC
        STA Counter2
        LDA Counter2
        STA TempI
        B FibRoutine

; -------------------
; Task3: tweak Counter3, then run Fib
; -------------------
Task3:
        LDA Counter3
        DEC
        STA Counter3
        LDA Counter3
        STA TempI
        B FibRoutine

; -------------------
; Task4: tweak Counter4, then run Fib
; -------------------
Task4:
        LDA Counter4
        ADD Counter1    ; just an example tweak
        STA Counter4
        LDA Counter4
        STA TempI
        B FibRoutine

; -------------------
; Shared Fibonacci routine
; Input: TempI = N
; Output: A = Fib(N), also stored back into the calling task's counter
; -------------------
FibRoutine:
        LDI Const0
        STA TempN1
        LDI Const1
        STA TempN2

FibLoop:
        LDA TempI
        BZ FibDone
        DEC TempI

        ; A = TempN1 + TempN2
        LDA TempN1
        LDX TempN2
        ADD
        STA TempN2

        ; A = TempN2 - TempN1  (old n2 - old n1)
        LDA TempN2
        LDX TempN1
        SUB
        STA TempN1

        B FibLoop

FibDone:
        LDA TempN1

        ; Compare TaskIndex to 0
        LDX Const0
        LDA TaskIndex
        SUBF
        BZ StoreC1

        ; Compare TaskIndex to 1
        LDX Const1
        LDA TaskIndex
        SUBF
        BZ StoreC2

        ; Compare TaskIndex to 2
        LDX Const2
        LDA TaskIndex
        SUBF
        BZ StoreC3

        ; Else -> TaskIndex == 3
        B StoreC4

StoreC1:
        STA Counter1
        B Yield
StoreC2:
        STA Counter2
        B Yield
StoreC3:
        STA Counter3
        B Yield
StoreC4:
        STA Counter4
        B Yield
