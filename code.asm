.org $0000

    ; A = mem[$1000]; loop: A += X; mem[$1000] = A; A--; if A != 0 -> loop
    LDA $1000
loop:
    ADD
    STA $1000
    DEC
    BNZ loop

    ; Show register moves
    XTA        ; A = X
    ATX        ; X = A
    BR done    ; short relative jump
done:
    B $0000    ; restart (just for demo)