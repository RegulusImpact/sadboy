// MBC.cpp

// uintX_t
#include "MBC.h"

MBC::MBC() {
}

MBC::~MBC() {
    delete cart;
}

bool MBC::LoadCart(std::string filePath) { // return cart->IsLoaded
    cart = new Cartridge(filePath);
    cartLoaded = cart->IsLoaded();

    if (cartLoaded && cart->HeaderChecksum()) {
        cart->DumpInfo();
        ParseCartProperties();
    }

    return IsCartLoaded();
}

bool MBC::IsCartLoaded() {
    return cartLoaded;
}

void MBC::ParseCartProperties() {
    // features

    switch (cart->cartType) {
        case 0x00: {
            romType = ROM_TYPE::ROM;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x01: {
            romType = ROM_TYPE::MBC1;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x02: {
            romType = ROM_TYPE::MBC1;
            cartRam = true;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x03: {
            romType = ROM_TYPE::MBC1;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        /*
        case 0x05: {
            romType = ROM_TYPE::MBC2;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x06: {
            romType = ROM_TYPE::MBC2;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x08: {
            romType = ROM_TYPE::ROM;
            cartRam = true;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x09: {
            romType = ROM_TYPE::ROM;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x0B: {
            romType = ROM_TYPE::MMM01;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x0C: {
            romType = ROM_TYPE::MMM01;
            cartRam = true;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x0D: {
            romType = ROM_TYPE::MMM01;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x0F: {
            romType = ROM_TYPE::MBC3;
            cartRam = false;
            cartBattery = true;
            cartTimer = true;
            cartRumble = false;
        }
            break;
        case 0x10: {
            romType = ROM_TYPE::MBC3;
            cartRam = true;
            cartBattery = true;
            cartTimer = true;
            cartRumble = false;
        }
            break;
        case 0x11: {
            romType = ROM_TYPE::MBC3;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x12: {
            romType = ROM_TYPE::MBC3;
            cartRam = true;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x13: {
            romType = ROM_TYPE::MBC3;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x19: {
            romType = ROM_TYPE::MBC5;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x1A: {
            romType = ROM_TYPE::MBC5;
            cartRam = true;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x1B: {
            romType = ROM_TYPE::MBC5;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x1C: {
            romType = ROM_TYPE::MBC5;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = true;
        }
            break;
        case 0x1D: {
            romType = ROM_TYPE::MBC5;
            cartRam = true;
            cartBattery = false;
            cartTimer = false;
            cartRumble = true;
        }
            break;
        case 0x1E: {
            romType = ROM_TYPE::MBC5;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = true;
        }
            break;
        case 0x20: {
            romType = ROM_TYPE::MBC6;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
        }
            break;
        case 0x22: {
            romType = ROM_TYPE::MBC7;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
            //cartAccel = true;
        }
            break;
        case 0xFE: {
            romType = ROM_TYPE::HuC3;
            cartRam = false;
            cartBattery = false;
            cartTimer = false;
            cartRumble = false;
            //cartAccel = true;
        }
            break;
        case 0xFF: {
            romType = ROM_TYPE::HuC1;
            cartRam = true;
            cartBattery = true;
            cartTimer = false;
            cartRumble = false;
            //cartAccel = true;
        }
            break;
            */
    }


    romSize = (ROM_SIZE)(cart->romSize);
    romBankSize = 0x4000;
    romBankSelect = 0x01;

    // x8000 == 32KB
    // x4000 == 16KB
    // x2000 == 8KB
    // x1000 == 4KB
    // x0800 == 2KB
    // ramSize = (RAM_SIZE)(cart->ramSize);
    // if (ramSize == RAM_SIZE::NORAM) {
    //     ramBankSize = 0;
    // } else if (ramSize == RAM_SIZE::RAM_2KB) {
    //     ramBankSize = 0x0800;
    // } else {
    //     ramBankSize = 0x2000;
    // }
    //
    // ramEnable = false;
    // ramBankSelect = 0x00;
    //
    bankSelectMode = false;
}

uint8_t MBC::Read(uint16_t addr) {
    // return cart->Read(addr);
    if (addr < SWITCH_ROM_START) {
        return cart->Read(addr);
    } else if (addr < SWITCH_ROM_END) {
        uint16_t selected = romBankSelect;

        if (bankSelectMode) {
            selected &= 0b00011111;
        }


        uint32_t sromAddr = (selected * 0x4000) + (addr - 0x4000);
        return cart->Read(sromAddr);
    }

    // else if (addr < SWITCH_RAM_END && addr >= SWITCH_RAM_START) {
    //     uint16_t selected = ramBankSelect;
    //     if (!bankSelectMode) {
    //         selected = 0;
    //     }
    //
    //     if (ramEnable) {
    //         uint32_t sramAddr = (selected * ramBankSize) + (addr - ramBankSize);
    //         return cart->ReadRAM(sramAddr);
    //     }
    // }

    return 0xFF;
}

void MBC::Write(uint16_t addr, uint8_t val) {
    // if the addr is in the base rom bank
    if (addr < SWITCH_ROM_START) {
        // if we have ram
        // and the addr we're writing to is in the ram-enable section
        // and the lower nibble is 0xA
        // then enable ram
        // else disable ram
        if (cartRam && addr < RAM_ENABLE_END && (0xA == (val & 0xF))) {
            ramEnable = true;
        } else if (cartRam && addr < RAM_ENABLE_END) {
            ramEnable = false;
        } else {// form 0x2000 to 0x3FFF select last 5 btis
            romBankSelect = (val & 0b00011111);
            // auto correct romBank0 to 01
            romBankSelect = romBankSelect == 0x00 ? 0x01 : romBankSelect;
        }
    }

    // else if (addr < SWITCH_ROM_END) {
    //     // depending on bankSelectMode, two bits are used to set the ram bank
    //     // or the upper 2 bits of the ROM bank select
    //     if (addr < 0x6000) {
    //         uint8_t bits = val & 3;
    //         // ram banking
    //         if (bankSelectMode) {
    //             ramBankSelect = bits;
    //         } else {
    //             // rom banking
    //             romBankSelect &= 0b10011111;
    //             romBankSelect |= (bits << 5);
    //         }
    //     } else {
    //         // this sets the bank select mode via 1 bit
    //         bankSelectMode = (val & 1) == 1;
    //     }
    // }
    //
    // else if (addr < SWITCH_RAM_END && addr > SWITCH_RAM_START) {
    //     uint16_t selected = ramBankSelect;
    //     if (!bankSelectMode) {
    //         selected = 0;
    //     }
    //
    //     if (ramEnable) {
    //         uint32_t sramAddr = (selected * ramBankSize) + (addr - ramBankSize);
    //         cart->WriteRAM(sramAddr, val);
    //     }
    // }
}
