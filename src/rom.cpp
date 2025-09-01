#include <cstdint>
#include <cstdio>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "rom.h"

Rom load_rom(const char* path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open ROM");
    std::vector<uint8_t> buf((std::istreambuf_iterator<char>(f)),
                              std::istreambuf_iterator<char>());
    if (buf.size() < 12) throw std::runtime_error("ROM too small");
    if (std::string((char*)buf.data(), 4) != "MR8C") throw std::runtime_error("Bad magic");
    uint8_t ver = buf[4];
    if (ver != 1) throw std::runtime_error("Unsupported ROM version");
    uint16_t origin = buf[6] << 8 | buf[5];
    uint16_t size   = buf[8] << 8 | buf[7];
    uint16_t csum   = buf[10] << 8 | buf[9];
    if (buf.size() != 12u + size) std::printf("Warning:ROM size mismatch\n");
    uint16_t calc = 0;
    for (size_t i = 12; i < buf.size(); ++i) calc = (calc + buf[i]) & 0xFFFF;
    if (calc != csum) std::printf("Warning:ROM checksum mismatch\n");
    Rom rom;
    rom.origin = origin;
    rom.data.assign(buf.begin() + 12, buf.end());
    return rom;
}

// Example integration:
// CPU cpu; Rom rom = load_rom("game.rom");
// std::copy(rom.data.begin(), rom.data.end(), cpu.mem + rom.origin);
// cpu.reset(rom.origin);