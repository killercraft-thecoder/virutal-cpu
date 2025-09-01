#pragma once
#include <cstdint>

static constexpr uint16_t STACK_BASE = 0x0200; // start of stack page

struct CPU
{
    // Registers
    uint8_t A = 0;
    uint8_t X = 0;
    uint8_t SP = 0xFF;
    uint16_t PC = 0;
    uint8_t P = 0;        // bit0=C, bit1=Z, bit6=V, bit7=N
    bool _halted = false; // for faster emualtion only

    // Memory
    uint8_t mem[65536]{};

    // Flag bits
    enum
    {
        C = 1 << 0,
        Z = 1 << 1,
        I = 1 << 2,
        H = 1 << 3,
        B = 1 << 4,
        U = 1 << 5,
        V = 1 << 6,
        N = 1 << 7
    };

    // Methods
    void reset(uint16_t start_addr);
    void step();
    void run(); // Run Until Halt

    // Helpers
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t val);
    void setNZ(uint8_t val);
    void setAddFlags(uint8_t a, uint8_t b, uint16_t res);
    void setSubFlags(uint8_t a, uint8_t b, uint16_t res);
    void push8(uint8_t value);
    uint8_t pop8();
};