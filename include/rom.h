#include <cstdint>
#include <vector>


struct Rom {
    uint16_t origin = 0;
    std::vector<uint8_t> data;
};

Rom load_rom(const char* path);
    