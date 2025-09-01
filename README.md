# Virtual CPU – Instruction Set Reference

This document lists all currently implemented opcodes for the Virtual CPU, their hex codes, sizes, and descriptions.

| Hex  | Mnemonic | Size (bytes) | Description |
|------|----------|--------------|-------------|
| 0x00 | NOP      | 1  | No operation. |
| 0x01 | LDA abs  | 3  | Load A from absolute address. |
| 0x02 | STA abs  | 3  | Store A to absolute address. |
| 0x03 | ADD      | 1  | A = A + X, set N/Z/C/V flags. |
| 0x04 | SUB      | 1  | A = A - X, set N/Z/C/V flags. |
| 0x05 | INC      | 1  | Increment A by 1, set N/Z flags. |
| 0x06 | DEC      | 1  | Decrement A by 1, set N/Z flags. |
| 0x07 | XTA      | 1  | A = X. |
| 0x08 | ATX      | 1  | X = A. |
| 0x09 | LDX abs  | 3  | Load X from absolute address. |
| 0x0A | STX abs  | 3  | Store X to absolute address. |
| 0x0B | BR rel   | 2  | Unconditional branch (relative). |
| 0x0C | BNZ abs  | 3  | Branch if Z=0 (absolute). |
| 0x0D | BZ abs   | 3  | Branch if Z=1 (absolute). |
| 0x0E | BN abs   | 3  | Branch if N=1 (absolute). |
| 0x0F | BP abs   | 3  | Branch if N=0 (absolute). |
| 0x10 | JSR abs  | 3  | Jump to subroutine (absolute). |
| 0x11 | RTS      | 1  | Return from subroutine. |
| 0x12 | BSR rel  | 2  | Branch to subroutine (relative). |
| 0x13 | BC abs   | 3  | Branch if C=1 (absolute). |
| 0x14 | BCR rel  | 2  | Branch if C=1 (relative). |
| 0x15 | BNR rel  | 2  | Branch if N=1 (relative). |
| 0x16 | BPR rel  | 2  | Branch if N=0 (relative). |
| 0x17 | BP abs   | 3  | Branch if N=0 (absolute). |
| 0x18 | BN abs   | 3  | Branch if N=1 (absolute). |
| 0x19 | XSRA     | 1  | Shift X right, store result in A, set N/Z/C. |
| 0x1A | XSLA     | 1  | Shift X left, store result in A, set N/Z/C. |
| 0x1B | ASRX     | 1  | Shift A right, store result in X, set N/Z/C. |
| 0x1C | ASLX     | 1  | Shift A left, store result in X, set N/Z/C. |
| 0x1D | AND      | 1  | A = A & X, set N/Z. |
| 0x1E | OR       | 1  | A = A \| X, set N/Z. |
| 0x1F | XOR/EOR  | 1  | A = A ^ X, set N/Z. |
| 0x20 | CLF      | 1  | Clear all flags. |
| 0x21 | CLC      | 1  | Clear carry flag. |
| 0x22 | CLN      | 1  | Clear negative flag. |
| 0x23 | CLZ      | 1  | Clear zero flag. |
| 0x24 | XXA      | 1  | X = X ^ A, set N/Z. |
| 0x25 | BRR rel  | 2  | Branch to subroutine (relative return address). |
| 0x26 | RTR      | 1  | Return from BRR. |
| 0x27 | BA       | 1  | Branch relative using signed offset in A. |
| 0x28 | ADDF     | 1  | Compute A+X, set flags only (no store). |
| 0x29 | SUBF     | 1  | Compute A-X, set flags only (no store). |
| 0xFF | HALT     | 1  | Halt execution. |

---

## Notes
- **abs** = absolute 16‑bit address operand.
- **rel** = signed 8‑bit offset relative to the next instruction.
- Flags:  
  - **N** = Negative  
  - **Z** = Zero  
  - **C** = Carry  
  - **V** = Overflow
- All arithmetic and logic ops update N/Z; ADD/SUB variants also update C/V.
