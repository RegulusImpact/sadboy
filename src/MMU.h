#ifndef MMU_H
#define MMU_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

// utility stuff
#include <random>
#include <iostream>
#include "string.h"

// classes
#include "Graphics/iGPU.h"
#include "Cartridge.h"
#include "Utils.h"

class MMU {
private:
// 16kB ROM bank #0
// 16kB switchable ROM bank
// 16kB switchable ROM bank
// 8kB video RAM
// 8kB switchable RAM bank
// 8kB internal RAM
// 8kB echo internal RAM
// 160B sprite ram
// 60B unusable for I/o
// 76B for I/o
// 60B unusable for I/o
// 128B internal RAM
// 1b

// design from cinoop
    std::uint8_t cart[0x8000];
    std::uint8_t sram[0x2000];
    std::uint8_t io[0x100]; // this contains timers, joypad, gpu stuff
    std::uint8_t vram[0x2000];
    std::uint8_t oam[0x100];
    std::uint8_t wram[0x2000];
    std::uint8_t hram[0x80];
    std::uint8_t interrupt;

    std::uint8_t bios[0xFF];

    const char* bootrom = "31FEFFAF21FF9F32CB7C20FB2126FF0E113E8032E20C3EF3E2323E77773EFCE0471104012110801ACD9500CD9600137BFE3420F311D80006081A1322230520F93E19EA1099212F990E0C3D2808320D20F92E0F18F3673E6457E0423E91E040041E020E0CF044FE9020FA0D20F71D20F20E13247C1E83FE6228061EC1FE6420067BE20C3E87F2F04290E0421520D205204F162018CB4F0604C5CB1117C1CB11170520F522232223C9CEED6666CC0D000B03730083000C000D0008111F8889000EDCCC6EE6DDDDD999BBBB67636E0EECCCDDDC999FBBB9333E3C42B9A5B9A5423C21040111A8001A13BE20FE237DFE3420F506197886230520FB8620FE3E01E050";

    const char* nintendoLogo = "CEED6666CC0D000B03730083000C000D0008111F8889000EDCCC6EE6DDDDD999BBBB67636E0EECCCDDDC999FBBB9333E";

    const char* emutitle = "42474257454C434F4D45000000000000";

    void loadBootrom() {
        uint16_t x = 0x0000;
        for(unsigned int i = 0; i < strlen(bootrom); i += 2) {
            uint8_t out = Utils::ParseCharsToHex(bootrom[i], bootrom[i + 1]);

            bios[x] = out;
            x++;
        }

        //loadDebugLogo();
    }

    void loadDebugLogo() {
        uint16_t x = 0x0104;
        for(unsigned int i = 0; i < strlen(nintendoLogo); i += 2) {
            uint8_t out = Utils::ParseCharsToHex(nintendoLogo[i], nintendoLogo[i + 1]);

            bios[x] = out;
            x++;
        }

        loadDebugTitle();
    }

    void loadDebugTitle() {
        uint16_t x = 0x0134;
        for(unsigned int i = 0; i < strlen(emutitle); i += 2) {
            uint8_t out = Utils::ParseCharsToHex(emutitle[i], emutitle[i + 1]);

            bios[x] = out;
            x++;
        }
    }
public:
    MMU(Cartridge* crt);
    ~MMU();

    bool readBios;
    bool enableDebugger;

// palettes
    std::uint8_t palette[4];
    std::uint8_t bgp[4];
    std::uint8_t obp[2][4];

    std::uint8_t tiles[384][8][8];

// functions
    std::uint8_t Read(uint16_t addr);
    std::uint16_t Read16Bit(uint16_t addr);
    void Write(uint16_t addr, uint8_t val);
    void Write(uint16_t addr, uint16_t val);

    void Copy(uint16_t destination, uint16_t source, size_t length);

    void UpdateTile(uint16_t address, uint8_t value);

    void CheckMemory();
};

#endif
