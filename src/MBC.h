// MBC.h
#ifndef MBC_H
#define MBC_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include "Cartridge.h"

class MBC {
private:
    enum ROM_TYPE {
        ROM,
        MBC1,
        MBC2,
        MMM01,
        MBC3,
        MBC5,
        MBC6,
        MBC7,
        HuC3,
        HuC1
    };
    enum ROM_SIZE {
        _32KB  = 0x00, // 32kb, no mbc needed
        _64KB  = 0x01, // 4 banks
        _128KB = 0x02, // 8 banks
        _256KB = 0x03, // 16 banks
        _512KB = 0x04, // 32 banks
        _1MB   = 0x05, // 64 banks
        _2MB   = 0x06, // 128 banks
        _4MB   = 0x07, // 256 banks
        _8MB   = 0x08, // 512 banks
    };
    enum RAM_SIZE {
        NORAM  = 0x00, // no ram
        RAM_2KB  = 0x01, // 1 of 2KB banks
        RAM_8KB = 0x02, // 1 of 8KB banks
        RAM_32KB = 0x03, // 4 of 8KB banks
        RAM_128KB = 0x04, // 16 of 8KB banks
        RAM_64KB   = 0x05, // 8 of 8KB banks
    };

    // features
    ROM_TYPE romType;
    ROM_SIZE romSize;
    RAM_SIZE ramSize;
    bool cartRam;
    bool cartBattery;
    bool cartTimer;
    bool cartRumble;

    // functionality
    bool ramEnable;
    uint16_t ramBankSize;
    uint16_t romBankSize;
    uint16_t romBankSelect;
    uint16_t ramBankSelect;
    bool bankSelectMode;

    Cartridge* cart;
    bool cartLoaded;

    const static uint16_t RAM_ENABLE_END = 0x2000;

    const static uint16_t SWITCH_ROM_START = 0x4000;
    const static uint16_t SWITCH_ROM_END= 0x8000;

    const static uint16_t SWITCH_RAM_START = 0xA000;
    const static uint16_t SWITCH_RAM_END = 0xC000;

public:
    MBC();
    ~MBC();

    bool LoadCart(std::string filePath); // return cart->IsLoaded
    bool IsCartLoaded();
    void ParseCartProperties();

    uint8_t Read(uint16_t addr);
    void Write(uint16_t addr, uint8_t val);
};

#endif
