# Virtual CPU – Instruction Set Reference

This document lists all **68** currently implemented opcodes for the Virtual CPU, along with their hex codes, instruction sizes, and descriptions for quick reference.

| Hex    | Mnemonic   | Size (bytes) | Description                                           | Base Cycles                                                                                                         | +1 if branch taken |
| ------ | ---------- | ------------ | ----------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------- | ------------------ |
| 0x00   | NOP        | 1            | No operation.                                         | 2                                                                                                                   | No                 |
| 0x01   | LDA abs    | 3            | Load A from absolute address.                         | 4                                                                                                                   | No                 |
| 0x02   | STA abs    | 3            | Store A to absolute address.                          | 5                                                                                                                   | No                 |
| 0x03   | ADD        | 1            | A = A + X, set N/Z/C/V flags.                         | 2                                                                                                                   | No                 |
| 0x04   | SUB        | 1            | A = A - X, set N/Z/C/V flags.                         | 2                                                                                                                   | No                 |
| 0x05   | INC        | 1            | Increment A by 1, set N/Z flags.                      | 2                                                                                                                   | No                 |
| 0x06   | DEC        | 1            | Decrement A by 1, set N/Z flags.                      | 2                                                                                                                   | No                 |
| 0x07   | XTA        | 1            | A = X.                                                | 2                                                                                                                   | No                 |
| 0x08   | ATX        | 1            | X = A.                                                | 2                                                                                                                   | No                 |
| 0x09   | LDX abs    | 3            | Load X from absolute address.                         | 4                                                                                                                   | No                 |
| 0x0A   | STX abs    | 3            | Store X to absolute address.                          | 5                                                                                                                   | No                 |
| 0x0B   | BR rel     | 2            | Unconditional branch (relative).                      | 3                                                                                                                   | No                 |
| 0x0C   | BNZ abs    | 3            | Branch if Z=0 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x0D   | BZ abs     | 3            | Branch if Z=1 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x0E   | BN abs     | 3            | Branch if N=1 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x0F   | BP abs     | 3            | Branch if N=0 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x10   | JSR abs    | 3            | Jump to subroutine (absolute).                        | 6                                                                                                                   | No                 |
| 0x11   | RTS        | 1            | Return from subroutine.                               | 4                                                                                                                   | No                 |
| 0x12   | BSR rel    | 2            | Branch to subroutine (relative).                      | 5                                                                                                                   | No                 |
| 0x13   | BC abs     | 3            | Branch if C=1 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x14   | BCR rel    | 2            | Branch if C=1 (relative).                             | 3                                                                                                                   | Yes                |
| 0x2A   | BNC abs    | 3            | Branch if C=0 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x2B   | BNCR rel   | 2            | Branch if C=0 (relative).                             | 3                                                                                                                   | Yes                |
| 0x15   | BNR rel    | 2            | Branch if N=1 (relative).                             | 3                                                                                                                   | Yes                |
| 0x16   | BPR rel    | 2            | Branch if N=0 (relative).                             | 3                                                                                                                   | Yes                |
| 0x17   | BP abs     | 3            | Branch if N=0 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x18   | BN abs     | 3            | Branch if N=1 (absolute).                             | 4                                                                                                                   | Yes                |
| 0x19   | XSRA       | 1            | Shift X right, store result in A, set N/Z/C.          | 3                                                                                                                   | No                 |
| 0x1A   | XSLA       | 1            | Shift X left, store result in A, set N/Z/C.           | 3                                                                                                                   | No                 |
| 0x1B   | ASRX       | 1            | Shift A right, store result in X, set N/Z/C.          | 3                                                                                                                   | No                 |
| 0x1C   | ASLX       | 1            | Shift A left, store result in X, set N/Z/C.           | 3                                                                                                                   | No                 |
| 0x1D   | AND        | 1            | A = A & X, set N/Z.                                   | 2                                                                                                                   | No                 |
| 0x1E   | OR         | 1            | A = A \| X, set N/Z.                                  | 2                                                                                                                   | No                 |
| 0x1F   | XOR/EOR    | 1            | A = A ^ X, set N/Z.                                   | 2                                                                                                                   | No                 |
| 0x20   | CLF        | 1            | Clear all flags.                                      | 2                                                                                                                   | No                 |
| 0x21   | CLC        | 1            | Clear carry flag.                                     | 2                                                                                                                   | No                 |
| 0x22   | CLN        | 1            | Clear negative flag.                                  | 2                                                                                                                   | No                 |
| 0x23   | CLZ        | 1            | Clear zero flag.                                      | 2                                                                                                                   | No                 |
| 0x24   | XXA        | 1            | X = X ^ A, set N/Z.                                   | 2                                                                                                                   | No                 |
| 0x25   | BRR rel    | 2            | Branch to subroutine (relative return address).       | 5                                                                                                                   | No                 |
| 0x26   | RTR        | 1            | Return from BRR.                                      | 4                                                                                                                   | No                 |
| 0x27   | BA         | 1            | Branch relative using signed offset in A.             | 3                                                                                                                   | No                 |
| 0x28   | ADDF       | 1            | Compute A+X, set flags only (no store).               | 2                                                                                                                   | No                 |
| 0x29   | SUBF       | 1            | Compute A-X, set flags only (no store).               | 2                                                                                                                   | No                 |
| 0x2C   | PHA        | 1            | Push A onto stack.                                    | 3                                                                                                                   | No                 |
| 0x2D   | PLA        | 1            | Pull (pop) into A from stack, set N/Z.                | 3                                                                                                                   | No                 |
| 0x2E   | PHX        | 1            | Push X onto stack.                                    | 3                                                                                                                   | No                 |
| 0x2F   | PLX        | 1            | Pull (pop) into X from stack, set N/Z.                | 3                                                                                                                   | No                 |
| 0x30   | NOTA       | 1            | Bitwise NOT (invert) A, set N/Z.                      | 2                                                                                                                   | No                 |
| 0x31   | NOTX       | 1            | Bitwise NOT (invert) X, set N/Z.                      | 2                                                                                                                   | No                 |
| 0x32   | NEG        | 1            | Two’s complement negate A (A = -A), set N/Z/C/V.      | 2                                                                                                                   | No                 |
| 0x33   | SWAP       | 1            | Swap A and X.                                         | 2                                                                                                                   | No                 |
| ------ | ---------- | ------       | --------                                              | -------------                                                                                                       |
| 0x36   | SETBRK     | 3            | 3                                                     | Store a 16‑bit absolute address into the CPU’s break target register. Used with BRK to jump out of loops or blocks. | No                 |
| 0x37   | BRK        | 1            | 1                                                     | Jump to the address stored by SETBRK. Does not push return address; acts like a structured break in C/C++.          | No                 |
| 0x38   | ROL A      | 1            | Rotate A left through carry. Sets N/Z/C.              | 2                                                                                                                   | No                 |
| 0x39   | ROR A      | 1            | Rotate A right through carry. Sets N/Z/C.             | 2                                                                                                                   | No                 |
| 0x3A | ASL A  | 1 | Arithmetic shift left A. Sets N/Z/C. | 2 | No |
| 0x3B | ASR A  | 1 | Arithmetic shift right A (preserve sign). Sets N/Z/C. | 2 | No |
| 0x3C | BVS abs | 3 | Branch if overflow set to absolute address. | 4 | No |
| 0x3D | BVC abs | 3 | Branch if overflow clear to absolute address. | 4 | No |
| 0x3E | BVS rel | 2 | Branch if overflow set by signed offset. | 3 | No |
| 0x3F | BVC rel | 2 | Branch if overflow clear by signed offset. | 3 | No |
| 0x40 | JSRI ptr | 3 | Jump to subroutine at address read from given pointer address. Pushes return address. | 7 | No |
| 0x41 | BX | 1 | Branch relative by signed offset in X register. | 2 | No |
| 0x42 | BAX | 1 | Jump absolute to address formed from X:high, A:low. | 2 | No |
| 0xFF   | HALT       | 1            | Halt execution.                                      2                                                                                                                   | No                 |

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
  - **I** — Interrupt disable flag _(currently unused)_. Intended to mask interrupt handling when implemented.
  - **B** — Break flag _(currently unused)_. Reserved for software breakpoints or system calls.

- **Flag updates**:
  - All arithmetic and logical operations update **N** and **Z**.
  - ADD/SUB and their variants also update **C** and **V**.
  - Flag‑only operations (e.g., `CLC`, `CLF`, `CLN`, `CLZ`) directly modify specific bits in `P` without affecting others.

## Clock Rate

In a hypothetical hardware implementation using technology comparable to late 1970s or early 1980s microprocessors, this CPU would be expected to operate reliably at approximately **3 MHz**. At this frequency, and given the average cycles per instruction for the current instruction set, the effective throughput would be on the order of 1 MIPS. This performance is consistent with higher‑end home computers of the early 1980s, while remaining well within the capabilities of period‑appropriate memory and peripheral interfaces.

## Performance

At the nominal clock rate of 3 MHz, and assuming an average instruction cost of 3–4 clock cycles, the CPU achieves an effective throughput of approximately **0.75–1.0 MIPS** (million instructions per second, this is approxmate and ausmes 3-4 clock cycles on avarge per isntrtion). This is comparable to the performance of higher‑end home computers of the early 1980s, while remaining well within the capabilities of period‑appropriate memory and peripheral hardware.

The table below shows estimated effective instruction throughput at several representative clock rates:

| Clock Rate | Avg. Cycles/Instr | Approx. MIPS |
| ---------- | ----------------- | ------------ |
| 1 MHz      | 3–4               | 0.25–0.33    |
| 2 MHz      | 3–4               | 0.50–0.66    |
| 3 MHz      | 3–4               | 0.75–1.00    |
| 4 MHz      | 3–4               | 1.00–1.33    |

## Historical Comparison

For context, the estimated performance of this CPU at 3 MHz (≈ 0.75–1.0 MIPS) places it in the same general class as many popular home computers of the early 1980s:

| CPU / System                    | Year | Clock Rate | Approx. MIPS |
| ------------------------------- | ---- | ---------- | ------------ |
| MOS 6502 (Apple II, C64)        | 1977 | 1.0 MHz    | ~0.3–0.5     |
| Zilog Z80 (TRS‑80, ZX Spectrum) | 1976 | 3.5 MHz    | ~0.8–1.0     |
| Motorola 68000 (Amiga, ST)      | 1984 | 7.16 MHz   | ~1.0–1.2     |
| **This CPU (hypothetical HW)**  | —    | 3.0 MHz    | ~0.75–1.0    |

While not matching the raw throughput of later 16/32‑bit processors, this design would have been competitive with — and in some cases faster than — many 8‑bit systems of its era. In the early 1980s, performance in the 1 MIPS range was considered high‑end for consumer hardware, approaching the capabilities of earlier workstation‑class machines.

## Design Era Context

A CPU of this performance class and clock rate would have been well‑suited to the demands of early‑ to mid‑1980s home computing. At approximately 1 MIPS, it could comfortably support text‑based applications, 2D tile‑based or sprite‑driven games, simple graphical user interfaces, and basic productivity software such as word processors and spreadsheets. Systems in this range were also capable of handling modest real‑time control tasks, educational software, and early networking or modem‑based communications. While not intended for heavy 3D graphics or large‑scale multitasking, such a processor would have provided a responsive, versatile platform for the era’s typical workloads.
