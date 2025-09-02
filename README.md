# Virtual CPU – Instruction Set Reference

This document lists all **53** currently implemented opcodes for the Virtual CPU, along with their hex codes, instruction sizes, and descriptions for quick reference.

| Hex  | Mnemonic | Size (bytes) | Description | Base Cycles | +1 if branch taken |
|------|----------|--------------|-------------|-------------|--------------------|
| 0x00 | NOP      | 1  | No operation. | 2 | No |
| 0x01 | LDA abs  | 3  | Load A from absolute address. | 4 | No |
| 0x02 | STA abs  | 3  | Store A to absolute address. | 5 | No |
| 0x03 | ADD      | 1  | A = A + X, set N/Z/C/V flags. | 2 | No |
| 0x04 | SUB      | 1  | A = A - X, set N/Z/C/V flags. | 2 | No |
| 0x05 | INC      | 1  | Increment A by 1, set N/Z flags. | 2 | No |
| 0x06 | DEC      | 1  | Decrement A by 1, set N/Z flags. | 2 | No |
| 0x07 | XTA      | 1  | A = X. | 2 | No |
| 0x08 | ATX      | 1  | X = A. | 2 | No |
| 0x09 | LDX abs  | 3  | Load X from absolute address. | 4 | No |
| 0x0A | STX abs  | 3  | Store X to absolute address. | 5 | No |
| 0x0B | BR rel   | 2  | Unconditional branch (relative). | 3 | No |
| 0x0C | BNZ abs  | 3  | Branch if Z=0 (absolute). | 4 | Yes |
| 0x0D | BZ abs   | 3  | Branch if Z=1 (absolute). | 4 | Yes |
| 0x0E | BN abs   | 3  | Branch if N=1 (absolute). | 4 | Yes |
| 0x0F | BP abs   | 3  | Branch if N=0 (absolute). | 4 | Yes |
| 0x10 | JSR abs  | 3  | Jump to subroutine (absolute). | 6 | No |
| 0x11 | RTS      | 1  | Return from subroutine. | 4 | No |
| 0x12 | BSR rel  | 2  | Branch to subroutine (relative). | 5 | No |
| 0x13 | BC abs   | 3  | Branch if C=1 (absolute). | 4 | Yes |
| 0x14 | BCR rel  | 2  | Branch if C=1 (relative). | 3 | Yes |
| 0x2A | BNC abs  | 3  | Branch if C=0 (absolute). | 4 | Yes |
| 0x2B | BNCR rel | 2  | Branch if C=0 (relative). | 3 | Yes |
| 0x15 | BNR rel  | 2  | Branch if N=1 (relative). | 3 | Yes |
| 0x16 | BPR rel  | 2  | Branch if N=0 (relative). | 3 | Yes |
| 0x17 | BP abs   | 3  | Branch if N=0 (absolute). | 4 | Yes |
| 0x18 | BN abs   | 3  | Branch if N=1 (absolute). | 4 | Yes |
| 0x19 | XSRA     | 1  | Shift X right, store result in A, set N/Z/C. | 3 | No |
| 0x1A | XSLA     | 1  | Shift X left, store result in A, set N/Z/C. | 3 | No |
| 0x1B | ASRX     | 1  | Shift A right, store result in X, set N/Z/C. | 3 | No |
| 0x1C | ASLX     | 1  | Shift A left, store result in X, set N/Z/C. | 3 | No |
| 0x1D | AND      | 1  | A = A & X, set N/Z. | 2 | No |
| 0x1E | OR       | 1  | A = A \| X, set N/Z. | 2 | No |
| 0x1F | XOR/EOR  | 1  | A = A ^ X, set N/Z. | 2 | No |
| 0x20 | CLF      | 1  | Clear all flags. | 2 | No |
| 0x21 | CLC      | 1  | Clear carry flag. | 2 | No |
| 0x22 | CLN      | 1  | Clear negative flag. | 2 | No |
| 0x23 | CLZ      | 1  | Clear zero flag. | 2 | No |
| 0x24 | XXA      | 1  | X = X ^ A, set N/Z. | 2 | No |
| 0x25 | BRR rel  | 2  | Branch to subroutine (relative return address). | 5 | No |
| 0x26 | RTR      | 1  | Return from BRR. | 4 | No |
| 0x27 | BA       | 1  | Branch relative using signed offset in A. | 3 | No |
| 0x28 | ADDF     | 1  | Compute A+X, set flags only (no store). | 2 | No |
| 0x29 | SUBF     | 1  | Compute A-X, set flags only (no store). | 2 | No |
| 0x2C | PHA      | 1  | Push A onto stack. | 3 | No |
| 0x2D | PLA      | 1  | Pull (pop) into A from stack, set N/Z. | 3 | No |
| 0x2E | PHX      | 1  | Push X onto stack. | 3 | No |
| 0x2F | PLX      | 1  | Pull (pop) into X from stack, set N/Z. | 3 | No |
| 0x30 | NOTA     | 1  | Bitwise NOT (invert) A, set N/Z. | 2 | No |
| 0x31 | NOTX     | 1  | Bitwise NOT (invert) X, set N/Z. | 2 | No |
| 0x32 | NEG      | 1  | Two’s complement negate A (A = -A), set N/Z/C/V. | 2 | No |
| 0x33 | SWAP     | 1  | Swap A and X. | 2 | No |
| 0xFF | HALT     | 1  | Halt execution. | 2 | No |

---

## Notes

- **Addressing modes**:  
  - **abs** — Absolute addressing; a full 16‑bit address operand is provided.  
  - **rel** — Relative addressing; an 8‑bit signed offset is applied to the program counter (PC) relative to the next instruction.

- **Processor status flags** (in the `P` register):  
  - **N** — Negative flag. Set if the result of the last operation has bit 7 = 1.  
  - **Z** — Zero flag. Set if the result of the last operation is zero.  
  - **C** — Carry flag. Set if an addition produces a carry out of bit 7, or a subtraction does not require a borrow.  
  - **V** — Overflow flag. Set if a signed addition or subtraction results in a value outside the range −128 to +127.  
  - **H** — Halt flag. Set when the CPU is halted; execution stops until reset.  
  - **I** — Interrupt disable flag *(currently unused)*. Intended to mask interrupt handling when implemented.  
  - **B** — Break flag *(currently unused)*. Reserved for software breakpoints or system calls.

- **Flag updates**:  
  - All arithmetic and logical operations update **N** and **Z**.  
  - ADD/SUB and their variants also update **C** and **V**.  
  - Flag‑only operations (e.g., `CLC`, `CLF`, `CLN`, `CLZ`) directly modify specific bits in `P` without affecting others.