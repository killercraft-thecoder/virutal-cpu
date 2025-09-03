; --- Main loop ---
BR start

; Task 1: increment counter at 0x0100
task1:
    LDA 0x0100
    ADD #1
    STA 0x0100
    BR ret1

; Task 2: increment counter at 0x0101
task2:
    LDA 0x0101
    ADD #1
    STA 0x0101
    BR ret2

; --- Scheduler ---
start:
    BR task1

ret1:
    BR task2

ret2:
    BR start