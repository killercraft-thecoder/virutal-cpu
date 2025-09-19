#include "cpu.h"
#include <stdio.h>

// Cycle counts for each opcode (0x00–0xFF)
// Unused opcodes default to 0 cycles for now.
static const uint8_t CYCLES[256] = {
    /*0x00*/ 2,  // NOP
    /*0x01*/ 4,  // LDA abs
    /*0x02*/ 5,  // STA abs
    /*0x03*/ 2,  // ADD (A = A + X)
    /*0x04*/ 2,  // SUB (A = A - X)
    /*0x05*/ 2,  // INC A
    /*0x06*/ 2,  // DEC A
    /*0x07*/ 2,  // XTA
    /*0x08*/ 2,  // ATX
    /*0x09*/ 4,  // LDX abs
    /*0x0A*/ 5,  // STX abs
    /*0x0B*/ 3,  // BR rel
    /*0x0C*/ 4,  // BNZ abs
    /*0x0D*/ 4,  // BZ abs
    /*0x0E*/ 4,  // BN abs
    /*0x0F*/ 4,  // BP abs
    /*0x10*/ 6,  // JSR abs
    /*0x11*/ 4,  // RTS
    /*0x12*/ 5,  // BSR rel
    /*0x13*/ 4,  // BC abs
    /*0x14*/ 3,  // BCR rel
    /*0x15*/ 3,  // BNR rel
    /*0x16*/ 3,  // BPR rel
    /*0x17*/ 4,  // BP abs
    /*0x18*/ 4,  // BN abs
    /*0x19*/ 3,  // XSRA
    /*0x1A*/ 3,  // XSLA
    /*0x1B*/ 3,  // ASRX
    /*0x1C*/ 3,  // ASLX
    /*0x1D*/ 2,  // AND
    /*0x1E*/ 2,  // OR
    /*0x1F*/ 2,  // XOR/EOR
    /*0x20*/ 2,  // CLF
    /*0x21*/ 2,  // CLC
    /*0x22*/ 2,  // CLN
    /*0x23*/ 2,  // CLZ
    /*0x24*/ 2,  // XXA
    /*0x25*/ 5,  // BRR rel
    /*0x26*/ 4,  // RTR
    /*0x27*/ 3,  // BA
    /*0x28*/ 2,  // ADDF
    /*0x29*/ 2,  // SUBF
    /*0x2A*/ 4,  // BNC abs
    /*0x2B*/ 3,  // BNCR rel
    /*0x2C*/ 3,  // PHA
    /*0x2D*/ 3,  // PLA
    /*0x2E*/ 3,  // PHX
    /*0x2F*/ 3,  // PLX
    /*0x30*/ 2,  // NOTA
    /*0x31*/ 2,  // NOTX
    /*0x32*/ 2,  // NEG
    /*0x33*/ 2,  // SWAP
    /*0x34*/ 2,  // (reserved/simple op)
    /*0x35*/ 2,  // (reserved/simple op)
    /*0x36*/ 3,  // (reserved/simple op)
    /*0x37*/ 2,  // (reserved/simple op)
    /*0x38*/ 2,  // ROL A
    /*0x39*/ 2,  // ROR A
    /*0x3A*/ 2,  // ASL A
    /*0x3B*/ 2,  // ASR A
    /*0x3C*/ 4,  // BVS abs
    /*0x3D*/ 4,  // BVC abs
    /*0x3E*/ 3,  // BVS rel
    /*0x3F*/ 3,  // BVC rel
    /*0x40*/ 6,  // JSRI
    /*0x41*/ 2,  // BX
    /*0x42*/ 3,  // BAX
    /*0x43*/ 2,  // (reserved/simple op)
    /*0x44*/ 18, // DECOD
    /*0x45*/ 14, // DECBIN
    /*0x46*/ 16, // ADDBCD
    /*0x47*/ 19, // SUBBCD
    /*0x48*/ 2,  // (reserved/simple op)
    /*0x49*/ 2,  // (reserved/simple op)
    /*0x4A*/ 2,  // (reserved/simple op)
    /*0x4B*/ 2,  // (reserved/simple op)
    /*0x4E*/ 2,  // (reserved/simple op)
    /*0x4F*/ 2,  // (reserved/simple op)
    // ... fill remaining unused opcodes with 2 cycles for now ...
    [0xFF] = 2 // HALT
};

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

// Reads the next byte from memory and increments PC
uint8_t CPU::fetch8()
{
    uint8_t value = read(PC);
    PC++;
    return value;
}

// Reads the next two bytes (little-endian) and increments PC by 2
uint16_t CPU::read16()
{
    uint8_t low = read(PC);
    uint8_t high = read(PC + 1);
    PC += 2;
    return static_cast<uint16_t>(low) | (static_cast<uint16_t>(high) << 8);
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

    if (cycles > 0)
    {
        cycles--;
        return;
    } // still penalty

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
        cycles++;
    }
    break;
    case 0x06: // BZ: branch if Z==1
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        if (P & Z)
            PC = uint16_t(lo) | (uint16_t(hi) << 8);
        cycles++;
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
            cycles++;
        }
    }
    break;

    case 0x14: // BNR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (P & N)
        { // Negative flag set
            PC = uint16_t(PC + off);
            cycles++;
        }
    }
    break;

    case 0x15: // BPR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (!(P & N))
        { // Negative flag clear
            PC = uint16_t(PC + off);
            cycles++;
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
            cycles++;
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
            cycles++;
        }
    }
    break;

    case 0x18: // BCR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (P & C)
        {
            PC = uint16_t(PC + off);
            cycles++;
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

    case 0x2A: // BNC abs16
    {
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        uint16_t addr = (uint16_t(hi) << 8) | lo;
        if (!(P & C))
        { // Carry clear
            PC = addr;
            cycles++;
        }
    }
    break;

    case 0x2B: // BNCR rel8
    {
        int8_t off = (int8_t)read(PC++);
        if (!(P & C))
        { // Carry clear
            PC = uint16_t(PC + off);
            cycles++;
        }
    }
    break;

    case 0x2C: // PHA
        push8(A);
        break;

    case 0x2D: // PLA
        A = pop8();
        setNZ(A);
        break;

    case 0x2E: // PHX
        push8(X);
        break;

    case 0x2F: // PLX
        X = pop8();
        setNZ(X);
        break;

    case 0x30: // NOTA
        A = ~A;
        setNZ(A);
        break;

    case 0x31: // NOTX
        X = ~X;
        setNZ(X);
        break;

    case 0x32: // NEG
    {
        uint8_t oldA = A;
        A = (~A) + 1;
        P &= ~(N | Z | C | V);
        if (A == 0)
            P |= Z;
        if (A & 0x80)
            P |= N;
        if (A != 0)
            P |= C; // carry set if not zero after negate
        if (((oldA ^ A) & 0x80) != 0)
            P |= V;
    }
    break;

    case 0x33: // SWAP
    {
        uint8_t tmp = A;
        A = X;
        X = tmp;
    }
    break;

    case 0x34:
    { // ADC #imm
        uint8_t val = read(PC++);
        uint16_t sum = uint16_t(A) + val + (P & C ? 1 : 0);
        setFlag(C, sum > 0xFF);
        uint8_t result = sum & 0xFF;
        setFlag(Z, result == 0);
        setFlag(N, result & 0x80);
        // Overflow: if sign of A == sign of val, but sign of result != sign of A
        setFlag(V, (~(A ^ val) & (A ^ result) & 0x80) != 0);
        A = result;
        break;
    }
    case 0x35:
    { // SBC #imm
        uint8_t val = read(PC++);
        uint16_t diff = uint16_t(A) - val - ((P & C) ? 0 : 1);
        setFlag(C, diff < 0x100); // C=1 if no borrow
        uint8_t result = diff & 0xFF;
        setFlag(Z, result == 0);
        setFlag(N, result & 0x80);
        setFlag(V, ((A ^ val) & (A ^ result) & 0x80) != 0);
        A = result;
        break;
    }

    case 0x36:
    { // SETBRK abs16
        uint8_t lo = read(PC++);
        uint8_t hi = read(PC++);
        break_addr = (uint16_t(hi) << 8) | lo;
        break;
    }
    case 0x37:
    { // BRK
        PC = break_addr;
        break;
    }

    case 0x38: // ROL A
    {
        uint8_t oldCarry = (P & C) ? 1 : 0;
        uint8_t newCarry = (A & 0x80) ? 1 : 0;
        A = (A << 1) | oldCarry;
        setFlag(C, newCarry);
        setFlag(Z, A == 0);
        setFlag(N, A & 0x80);
        break;
    }

    case 0x39: // ROR A
    {
        uint8_t oldCarry = (P & C) ? 1 : 0;
        uint8_t newCarry = (A & 0x01) ? 1 : 0;
        A = (A >> 1) | (oldCarry << 7);
        setFlag(C, newCarry);
        setFlag(Z, A == 0);
        setFlag(N, A & 0x80);
        break;
    }

    case 0x3A: // ASL A
    {
        uint8_t newCarry = (A & 0x80) ? 1 : 0;
        A <<= 1;
        setFlag(C, newCarry);
        setFlag(Z, A == 0);
        setFlag(N, A & 0x80);
        break;
    }

    case 0x3B: // ASR A (Arithmetic Shift Right)
    {
        uint8_t newCarry = (A & 0x01) ? 1 : 0;
        // Preserve sign bit for arithmetic shift
        uint8_t sign = A & 0x80;
        A = (A >> 1) | sign;
        setFlag(C, newCarry);
        setFlag(Z, A == 0);
        setFlag(N, A & 0x80);
        break;
    }

    case 0x3C: // BVS abs
    {
        uint16_t addr = read16();
        if (P & V)
        {
            PC = addr;
        }
        break;
    }

    case 0x3D: // BVC abs
    {
        uint16_t addr = read16();
        if (!(P & V))
        {
            PC = addr;
        }
        break;
    }

    case 0x3E: // BVS rel
    {
        int8_t offset = (int8_t)fetch8();
        if (P & V)
        {
            PC += offset;
        }
        break;
    }

    case 0x3F: // BVC rel
    {
        int8_t offset = (int8_t)fetch8();
        if (!(P & V))
        {
            PC += offset;
        }
        break;
    }
    case 0x40: // JSRI (Jump to SubRoutine Indirect)
    {
        // Step 1: Fetch pointer address from instruction stream
        uint16_t ptrAddr = read16(); // increments PC by 2

        // Step 2: Read actual target address from memory at ptrAddr
        uint16_t targetAddr = read(ptrAddr) | (read(ptrAddr + 1) << 8);

        // Step 3: Push return address (PC - 1, like JSR)
        uint16_t returnAddr = PC;        // PC already points after operand
        push8((returnAddr >> 8) & 0xFF); // high byte
        push8(returnAddr & 0xFF);        // low byte

        // Step 4: Jump to target
        PC = targetAddr;
        break;
    }
    case 0x41: // BX - Branch relative using X
    {
        int8_t offset = static_cast<int8_t>(X);
        PC += offset;
        break;
    }
    case 0x42: // BAX - Jump absolute using A (low) and X (high)
    {
        uint16_t target = (static_cast<uint16_t>(X) << 8) | A;
        PC = target;
        break;
    }

    case 0x43: // Worko n this later.
    {
        this->P = 0;
        this->A = 0;
        this->X = 0;
        this->break_addr = 0;
        break;
    }

    case 0x44: // DECOD (Bin -> BCD)
    {
        uint8_t value = this->A;
        uint8_t bcd = 0;
        bool overflow = false;

        if (value > 99)
        {
            overflow = true; // can't represent >99 in 2-digit BCD
            value = 99;      // clamp to max BCD
        }

        // Fast conversion without modulo
        uint8_t tens = 0;
        while (value >= 10)
        {
            value -= 10;
            ++tens;
        }

        bcd = (tens << 4) | value;
        this->A = bcd;

        // Flag updates
        setFlag(Z, (bcd == 0)); // Zero flag
        setFlag(C, overflow);   // Carry flag signals overflow from binary to BCD

        break;
    }

    case 0x45: // DECBIN (BCD -> Bin)
    {
        uint8_t bcd = this->A;
        uint8_t tens = (bcd >> 4) & 0x0F;
        uint8_t ones = bcd & 0x0F;
        bool overflow = false;

        if (tens > 9 || ones > 9)
        {
            overflow = true; // invalid BCD digits
            tens = (tens > 9) ? 9 : tens;
            ones = (ones > 9) ? 9 : ones;
        }

        uint8_t binary = tens * 10 + ones;
        this->A = binary;

        // Flag updates
        setFlag(Z, (binary == 0)); // Zero flag
        setFlag(C, overflow);      // Carry flag signals invalid BCD input

        break;
    }

    case 0x46: // ADDBCD
    {
        uint8_t a = this->A;
        uint8_t b = this->X; // Assume second operand is in X
        uint8_t low = (a & 0x0F) + (b & 0x0F);
        uint8_t high = (a >> 4) + (b >> 4);
        bool carry = false;

        if (low > 9)
        {
            low -= 10;
            high += 1;
        }

        if (high > 9)
        {
            high = 9; // Clamp to max BCD
            carry = true;
        }

        this->A = (high << 4) | (low & 0x0F);

        // Flag updates
        setFlag(Z, (this->A == 0));
        setFlag(C, carry);
        setFlag(N, (this->A & 0x80) != 0);
        setFlag(V, carry); // Treat overflow as V flag

        break;
    }

    case 0x47: // SUBBCD
    {
        uint8_t a = this->A;
        uint8_t b = this->X; // Assume second operand is in X
        int8_t low = (a & 0x0F) - (b & 0x0F);
        int8_t high = (a >> 4) - (b >> 4);
        bool borrow = false;

        if (low < 0)
        {
            low += 10;
            high -= 1;
        }

        if (high < 0)
        {
            high = 0; // Clamp to zero
            borrow = true;
        }

        this->A = ((high & 0x0F) << 4) | (low & 0x0F);

        // Flag updates
        setFlag(Z, (this->A == 0));
        setFlag(C, !borrow); // Carry clear means borrow occurred
        setFlag(N, (this->A & 0x80) != 0);
        setFlag(V, borrow); // Treat borrow as overflow

        break;
    }

    case 0xFF:
        _halted = true;
        P |= H; // set Halt flag
        printf("[HALT] Invalid opcode 0x%02X at address 0x%04X\n", op, PC);
        break;
    default:
        // NOP for unknown opcodes
        break;
    }
    // Add base cycles from the table
    cycles += CYCLES[op];
}

inline void CPU::setFlag(int flag, bool cond)
{
    if (cond)
    {
        P |= flag;
    }
    else
    {
        P &= ~flag;
    }
}