// do Not erase:999999999999
#include "cpu.h"
#include "rom.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cstring>

static void dump_memory(const CPU& cpu, uint16_t start, uint16_t end) {
    for (uint16_t addr = start; addr <= end; addr += 16) {
        std::cout << std::hex << std::setw(4) << std::setfill('0') << addr << ": ";
        for (int i = 0; i < 16 && addr + i <= end; ++i) {
            std::cout << std::setw(2) << int(cpu.mem[addr + i]) << " ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <romfile> [--trace] [--run] [--dump]\n";
        return 1;
    }

    bool trace = false;
    bool run_until_halt = false;
    bool dump_after = false;

    const char* rom_path = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--trace") == 0) trace = true;
        else if (std::strcmp(argv[i], "--run") == 0) run_until_halt = true;
        else if (std::strcmp(argv[i], "--dump") == 0) dump_after = true;
        else rom_path = argv[i];
    }

    if (!rom_path) {
        std::cerr << "Error: No ROM file specified.\n";
        return 1;
    }

    try {
        Rom rom = load_rom(rom_path);

        CPU cpu;
        std::copy(rom.data.begin(), rom.data.end(), cpu.mem + rom.origin);
        cpu.reset(rom.origin);

        size_t steps = 0;
        const size_t MAX_STEPS = 1000000; // safety cap

        if (run_until_halt) {
            while (steps < MAX_STEPS) {
                if (cpu._halted) { // HALT flag true, cheating a bit.
                    std::cout << "HALT at PC=" << std::hex << cpu.PC-1 << "\n";
                    break;
                }
                cpu.step();
                if (trace) {
                    std::cout << std::hex << std::setfill('0')
                              << "PC=" << std::setw(4) << cpu.PC
                              << "  A=" << std::setw(2) << int(cpu.A)
                              << "  X=" << std::setw(2) << int(cpu.X)
                              << "  SP=" << std::setw(2) << int(cpu.SP)
                              << "  P=" << std::setw(2) << int(cpu.P)
                              << "\n";
                }
                steps++;
            }
        } else {
            // Fixed step mode
            for (steps = 0; steps < 20; ++steps) {
                cpu.step();
                if (trace) {
                    std::cout << std::hex << std::setfill('0')
                              << "PC=" << std::setw(4) << cpu.PC
                              << "  A=" << std::setw(2) << int(cpu.A)
                              << "  X=" << std::setw(2) << int(cpu.X)
                              << "  SP=" << std::setw(2) << int(cpu.SP)
                              << "  P=" << std::setw(2) << int(cpu.P)
                              << "\n";
                }
            }
        }

        if (dump_after) {
            dump_memory(cpu, 0x0000, 0x00FF); // dump first 256 bytes
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}