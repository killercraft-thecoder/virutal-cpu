        .org $0000          ; Program origin — CPU will start executing here

; ------------------------------------------------------------
; Demo 1: Simple loop
; Goal:
;   1. Load A from memory at $1000
;   2. Loop:
;        A = A + X
;        Store A into $9000
;        Decrement A
;        If A != 0, repeat loop
; ------------------------------------------------------------

        LDA $1000           ; Load A with value from RAM[$1000]

loop:
        ADD                 ; A = A + X   (X is unchanged here)
        STA $9000           ; Store A into RAM[$9000]
        DEC                 ; A = A - 1
        BNZ loop            ; Branch if A != 0 (NZ flag set) back to 'loop'

; ------------------------------------------------------------
; Demo 2: Register moves
; Show how to copy between A and X
; ------------------------------------------------------------

        XTA                 ; Copy X into A  (A = X)
        ATX                 ; Copy A into X  (X = A)

; ------------------------------------------------------------
; Demo 3: Short jump
; ------------------------------------------------------------

        BR done             ; Relative branch to 'done' label

done:
        B $0000             ; Absolute branch to $0000 — restart program