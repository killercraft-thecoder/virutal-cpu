#include "cpu.h"

void CPU::reset(uint16_t start_addr)
{
    A = X = 0;
    SP = 0xFF;
    P = 0;
    PC = start_addr;
    _halted = false;
    P &= ~H;
}

uint8_t CPU::read(uint16_t addr) const
{
    return mem[addr];
}

void CPU::write(uint16_t addr, uint8_t val)
{
    mem[addr] = val;
}

void CPU::setNZ(uint8_t val)
{
    if (val == 0)
        P |= Z;
    else
        P &= ~Z;
    if (val & 0x80)
        P |= N;
    else
        P &= ~N;
}

void CPU::setAddFlags(uint8_t a, uint8_t b, uint16_t res)
{
    setNZ(uint8_t(res));
    if (res > 0xFF)
        P |= C;
    else
        P &= ~C;
    uint8_t r = uint8_t(res);
    if (~(a ^ b) & (a ^ r) & 0x80)
        P |= V;
    else
        P &= ~V;
}

void CPU::setSubFlags(uint8_t a, uint8_t b, uint16_t res)
{
    setNZ(uint8_t(res));
    if (res < 0x100)
        P |= C;
    else
        P &= ~C; // carry = no borrow
    uint8_t r = uint8_t(res);
    if ((a ^ b) & (a ^ r) & 0x80)
        P |= V;
    else
        P &= ~V;
}

/**
 * @struct
 * @short Run Until Halt
 */
void CPU::run()
{
    while (!_halted || PC == 65535)
    {
        step();
    }
}

void CPU::step()
{
    if (_halted)
        return;
    uint8_t op = read(PC++);
    switch (op)
    {
    case 0x00: // ADD: A = A + X
    {
        uint16_t res = uint16_t(A) + uint16_t(X);
        setAddFlags(A, X, res);
        A = uint8_t(res);
    }
    break;
    case 0x01: // SUB: A = A - X
    {
        uint16_t res = uint16_t(A) - uint16_t(X);
        setSubFlags(A, X, res & 0x1FF);
        A = uint8_t(res);
    }
    break;
    case 0x02: // INC: A++
        A++;
        setNZ(A);
        break;
    case 0x03: // DEC: A--
        A--;
        setNZ(A);
        break;
    case 0x04: // B: branch absolute
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        PC = uint16_t(lo) | (uint16_t(hi) << 8);
    }
    break;
    case 0x05: // BNZ: branch if Z==0
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        if (!(P & Z))
            PC = uint16_t(lo) | (uint16_t(hi) << 8);
    }
    break;
    case 0x06: // BZ: branch if Z==1
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        if (P & Z)
            PC = uint16_t(lo) | (uint16_t(hi) << 8);
    }
    break;
    case 0x07: // STX: SP -> X
        X = SP;
        setNZ(X);
        break;
    case 0x08: // XTS: X -> SP
        SP = X;
        break;
    case 0x09: // LDA abs
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        A = read((hi << 8) | lo);
        setNZ(A);
    }
    break;

    case 0x0A: // STA abs
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        write((hi << 8) | lo, A);
    }
    break;

    case 0x0B: // BR rel
    {
        int8_t off = (int8_t)read(PC++);
        PC = uint16_t(PC + off);
    }
    break;

    case 0x0C: // XTA
        A = X;
        setNZ(A);
        break;

    case 0x0D: // ATX
        X = A;
        setNZ(X); // odd behavor , but we can roll with it , becuase 6502 is odd too.
        break;

    case 0x0E: // LDX abs
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        X = read((hi << 8) | lo);
        setNZ(X);
    }
    break;

    case 0x0F: // STX abs
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        write((hi << 8) | lo, X);
    }
    break;

    case 0x10: // JSR abs
    {
        uint8_t jlo = read(PC++);
        uint8_t jhi = read(PC++);
        uint16_t target = (uint16_t(jhi) << 8) | jlo;

        // Push return address = address of last byte of JSR
        uint16_t ret = PC - 1;
        push8((ret >> 8) & 0xFF); // high byte first or low byte first depending on your convention
        push8(ret & 0xFF);

        PC = target;
    }
    break;

    case 0x11: // RTS
    {
        uint8_t lo = pop8();
        uint8_t hi = pop8();
        PC = ((uint16_t(hi) << 8) | lo) + 1; // +1 to move past the JSR
    }
    break;

    case 0x12: // BSR (Branch to SubRoutine, relative)
    {
        int8_t offset = (int8_t)read(PC++);
        uint16_t target = PC + offset;

        // Push return address (address of last byte of BSR)
        uint16_t ret = PC; // return to instruction after BSR
        push8((ret >> 8) & 0xFF);
        push8(ret & 0xFF);

        PC = target;
    }
    break;

    case 0x13: // BN abs16
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        uint16_t addr = (uint16_t(hi) << 8) | lo;
        if (P & N)
        { // Negative flag set
            PC = addr;
        }
    }
    break;

    case 0x14: // BNR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (P & N)
        { // Negative flag set
            PC = uint16_t(PC + off);
        }
    }
    break;

    case 0x15: // BPR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (!(P & N))
        { // Negative flag clear
            PC = uint16_t(PC + off);
        }
    }
    break;

    case 0x16: // BP abs16
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        uint16_t addr = (uint16_t(hi) << 8) | lo;
        if (!(P & N))
        { // N clear
            PC = addr;
        }
    }
    break;

    case 0x17: // BC abs16
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        uint16_t addr = (uint16_t(hi) << 8) | lo;
        if (P & C)
        {
            PC = addr;
        }
    }
    break;

    case 0x18: // BCR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (P & C)
        {
            PC = uint16_t(PC + off);
        }
    }
    break;

    case 0x19: // XSRA
    {
        uint8_t val = X;
        uint8_t carry = val & 0x01;
        val >>= 1;
        A = val;
        P = (P & ~(N | Z | C)) | (carry ? C : 0);
        if (A == 0)
            P |= Z;
        if (A & 0x80)
            P |= N;
    }
    break;

    case 0x1A: // XSLA
    {
        uint8_t val = X;
        uint8_t carry = (val >> 7) & 0x01;
        val <<= 1;
        A = val;
        P = (P & ~(N | Z | C)) | (carry ? C : 0);
        if (A == 0)
            P |= Z;
        if (A & 0x80)
            P |= N;
    }
    break;

    case 0x1B: // ASRX
    {
        uint8_t val = A;
        uint8_t carry = val & 0x01;
        val >>= 1;
        X = val;
        P = (P & ~(N | Z | C)) | (carry ? C : 0);
        if (X == 0)
            P |= Z;
        if (X & 0x80)
            P |= N;
    }
    break;

    case 0x1C: // ASLX
    {
        uint8_t val = A;
        uint8_t carry = (val >> 7) & 0x01;
        val <<= 1;
        X = val;
        P = (P & ~(N | Z | C)) | (carry ? C : 0);
        if (X == 0)
            P |= Z;
        if (X & 0x80)
            P |= N;
    }
    break;

    case 0x1D: // AND
        A = A & X;
        setNZ(A);
        break;

    case 0x1E: // OR
        A = A | X;
        setNZ(A);
        break;

    case 0x1F: // XOR
        A = A ^ X;
        setNZ(A);
        break;
    case 0x20: // CLF
        P = 0;
        break;

    case 0x21: // CLC
        P &= ~C;
        break;

    case 0x22: // CLN
        P &= ~N;
        break;

    case 0x23: // CLZ
        P &= ~Z;
        break;

    case 0x24: // XXA
    {
        X = X ^ A;
        setNZ(X);
    }
    break;

    case 0x25: // BRR rel8
    {
        int8_t offset = (int8_t)read(PC++);
        int8_t ret_offset = 0; // distance to return point

        // Return point is current PC (after reading operand)
        // relative to the target
        ret_offset = -offset; // so RTR can add it back

        push8((uint8_t)ret_offset); // store as unsigned byte
        PC = uint16_t(PC + offset);
    }
    break;

    case 0x26: // RTR
    {
        int8_t ret_offset = (int8_t)pop8();
        PC = uint16_t(PC + ret_offset);
    }
    break;

    case 0x27: // BA (Branch relative using A)
    {
        int8_t offset = (int8_t)A;
        PC = uint16_t(PC + offset);
    }
    break;

    case 0x28: // ADDF
    {
        uint16_t tmp = (uint16_t)A + (uint16_t)X;
        uint8_t result = (uint8_t)tmp;

        // Set flags based on result
        P &= ~(N | Z | C | V);
        if (result == 0)
            P |= Z;
        if (result & 0x80)
            P |= N;
        if (tmp > 0xFF)
            P |= C;
        // Overflow: sign of A == sign of X, but sign of result != sign of A
        if (((A ^ result) & (X ^ result) & 0x80) != 0)
            P |= V;
    }
    break;

    case 0x29: // SUBF
    {
        uint16_t tmp = (uint16_t)A - (uint16_t)X;
        uint8_t result = (uint8_t)tmp;

        P &= ~(N | Z | C | V);
        if (result == 0)
            P |= Z;
        if (result & 0x80)
            P |= N;
        if (A >= X)
            P |= C; // carry = no borrow
        // Overflow: sign of A != sign of X, and sign of result != sign of A
        if (((A ^ X) & (A ^ result) & 0x80) != 0)
            P |= V;
    }
    break;

    case 0xFF:
        _halted = true;
        P |= H; // set Halt flag
        break;
    default:
        // NOP for unknown opcodes
        break;
    }
}