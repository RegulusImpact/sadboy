//MMU.cpp

#include "MMU.h"

MMU::MMU(Cartridge* crt):
sprites(40, Sprite(-1))
{
    for (uint32_t ii = 0; ii < 0x8000; ii++) {
        cart[ii] = crt->Read(ii);
    }

    palette[0] = 0;
    palette[1] = 1;
    palette[2] = 2;
    palette[3] = 3;

    readBios = true;
    enableDebugger = false;

    Write(0xFF00, (uint8_t)0x1);
    Write(0xFF41, (uint8_t)3);

    loadBootrom();
}

std::uint8_t MMU::Read(uint16_t addr) {
    // cartridge / bios
    if (addr <= 0x7FFF) {
        if (readBios && addr < 0xFF)
            return bios[addr];
        else
            return cart[addr];
    }
    // sram
    else if (addr >= 0xA000 && addr <= 0xBFFF)
        return sram[addr - 0xA000];
    // video ram
    else if (addr >= 0x8000 && addr <= 0x9FFF) {
        return vram[addr - 0x8000];
    }

    // writable ram
    else if (addr >= 0xC000 && addr <= 0xDFFF)
        return wram[addr - 0xC000];

    // more writable ram
    else if (addr >= 0xE000 && addr <= 0xFDFF)
        return wram[addr - 0xE000];

    // oam
    else if (addr >= 0xFE00 && addr <= 0xFEFF)
        return oam[addr - 0xFE00];

    // io
    // this includes FFOO - user input joypad
    // timers
    // gpu

    else if (addr >= 0xFF80 && addr <= 0xFFFE)
        return hram[addr - 0xFF80];
    else if (addr >= 0xFF00 && addr <= 0xFF7F) {
        return io[(uint8_t)(addr - 0xFF00)];
    }
    else if (addr == 0xFFFF)
        return interrupt;

    return 0;
}

std::uint16_t MMU::Read16Bit(uint16_t addr) {
    uint16_t value = 0;

    uint16_t lo = (uint16_t)Read(addr);
    uint16_t hi = (uint16_t)Read(addr+1);
    value = (hi << 8) | lo;
    return value;
}

void MMU::Write(uint16_t addr, uint8_t val) {
    // RAM[addr] = val;


    // cartridge / bios
    // we don't write to cart
    //     cart[addr] = val;
    // sram
    if (addr >= 0xA000 && addr <= 0xBFFF)
        sram[addr - 0xA000] = val;
    // video ram
    else if (addr >= 0x8000 && addr <= 0x9FFF) {
        vram[addr - 0x8000] = val;
        if (addr <= 0x97FF) {
            UpdateTile(addr);
        }
    }

    // writable ram
    else if (addr >= 0xC000 && addr <= 0xDFFF)
        wram[addr - 0xC000] = val;

    // more writable ram
    else if (addr >= 0xE000 && addr <= 0xFDFF)
        wram[addr - 0xE000] = val;

    // oam
    else if (addr >= 0xFE00 && addr <= 0xFEFF){
        uint8_t effectiveAddress = (addr - 0xFE00);
        oam[effectiveAddress] = val;
        UpdateSprite(effectiveAddress, val);
    }

    // io
    // this includes FFOO - user input joypad
    // timers
    // gpu

    else if (addr >= 0xFF80 && addr <= 0xFFFE)
        hram[addr - 0xFF80] = val;

    else if (addr >= 0xFF00 && addr <= 0xFF7F) {
        // io[addr - 0xFF00] = val;

        if (addr == 0xFF00)
            io[addr - 0xFF00] = val | 0b11000000; // unused bits

        else if (addr == 0xFF02)
            io[addr - 0xFF00] = val | 0b01111110; // unused bits

        else if (addr == 0xFF03)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        // stupid timers div and tima
        else if (addr == 0xFF04) {
            io[0xFF04 - 0xFF00] = 0;
            io[0xFF05 - 0xFF00] = 0;
        }

        else if (addr == 0xFF07)
            io[addr - 0xFF00] = val | 0b11111000; // unused bits

        else if (addr >= 0xFF08 && addr < 0xFF0F)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else if (addr == 0xFF0F) {
            if (Read(0xFFFF) > 0) {
                io[addr - 0xFF00] = val | 0b11100000; // unused bits
            }
        }

        else if (addr == 0xFF10)
            io[addr - 0xFF00] = val | 0b10000000; // unused bits

        else if (addr == 0xFF15)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else if (addr == 0xFF1A)
            io[addr - 0xFF00] = val | 0b01111111; // unused bits

        else if (addr == 0xFF1C)
            io[addr - 0xFF00] = val | 0b10011111; // unused bits

        else if (addr == 0xFF1F)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else if (addr == 0xFF20)
            io[addr - 0xFF00] = val | 0b11000000; // unused bits

        else if (addr == 0xFF23)
            io[addr - 0xFF00] = val | 0b00111111; // unused bits

        else if (addr == 0xFF26)
            io[addr - 0xFF00] = val | 0b01110000; // unused bits

        else if (addr == 0xFF27)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else if (addr == 0xFF28)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else if (addr == 0xFF29)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else if (addr == 0xFF40)
            io[addr - 0xFF00] = val;

        else if (addr == 0xFF41)
            io[addr - 0xFF00] = val | 0b10000000; // unused bits

        else if (addr == 0xFF42)
            io[addr - 0xFF00] = val;

        else if (addr == 0xFF43)
            io[addr - 0xFF00] = val;

        else if (addr == 0xFF46) {
            uint32_t eAddr = val << 8;
            for (size_t ii = 0; ii < 160; ii++) {
                uint16_t oamAddr = (0xFE00 + ii);
                uint8_t wramVal = Read(eAddr + ii);
                Write(oamAddr, wramVal);
            }
        }

        else if (addr == 0xFF47) { // setup bgp
            for (int ii = 0; ii < 4; ii++) {
                bgp[ii] = palette[(val >> ( ii * 2)) & 3];
            }

            io[addr - 0xFF00] = val;
        }

        else if (addr == 0xFF48) { // setup OBP0 ; sprites
            for (int ii = 0; ii < 4; ii++) {
                obp[0][ii] = palette[(val >> ( ii * 2)) & 3];
            }

            io[addr - 0xFF00] = val;
        }

        else if (addr == 0xFF49) { // setup OBP1 ; sprites
            for (int ii = 0; ii < 4; ii++) {
                obp[1][ii] = palette[(val >> ( ii * 2)) & 3];
            }

            io[addr - 0xFF00] = val;
        }

        else if (addr >= 0xFF4C && addr < 0xFF80)
            io[addr - 0xFF00] = val | 0b11111111; // unused bits

        else {
            io[addr - 0xFF00] = val;
        }

    }
    else if (addr == 0xFFFF)
        interrupt = val;

    if (0xFF50 == addr && 1 == val) {
        readBios = false;
    }
}

// should literally be the opposite of the read16bit
void MMU::Write(uint16_t addr, uint16_t val) {
    // RAM[addr] = (uint8_t)((val & 0xFF00) >> 8);
    // RAM[(addr-1)] = (uint8_t)(val & 0x00FF);

    Write(addr, (uint8_t)((val & 0xFF00) >> 8));
    Write((addr-1), (uint8_t)(val & 0x00FF));
}

void MMU::WriteTimers(uint16_t addr, uint16_t val) {
    if (addr == 0xFF04 || addr == 0xFF05) {
        io[addr - 0xFF00] = val;
    }
}

void MMU::Copy(uint16_t destination, uint16_t source, size_t length) {
    for (size_t ii = 0; ii < length; ii++) {
        Write(destination + ii, Read(source + ii));
    }
}

void MMU::UpdateTile(uint16_t address) {
	address &= 0x1FFE;

	uint16_t tile = (address >> 4) & 511;
	uint16_t y = (address >> 1) & 7;

	uint8_t x, bitIndex;
	for(x = 0; x < 8; x++) {
		bitIndex = 1 << (7 - x);

		//((unsigned char (*)[8][8])tiles)[tile][y][x] = ((vram[address] & bitIndex) ? 1 : 0) + ((vram[address + 1] & bitIndex) ? 2 : 0);
		tiles[tile][y][x] = ((vram[address] & bitIndex)     != 0 ? 1 : 0) +
                            ((vram[address + 1] & bitIndex) != 0 ? 2 : 0);
	}
}


void MMU::UpdateSprite(uint8_t address, uint8_t val) {
    uint8_t obj = address >> 2;
    if (obj < 40) {
        switch (address & 3) {
            case 0: {
                int16_t y = val - 16;
                sprites[obj].SetY(y);
                // printf("\tset sprite (x%.4X) y from %d to %d | check: %s\n",address,oldy,y, (sprites[obj].GetY() == y) ? "true" : "false");
            }
                break;
            case 1: {
                int16_t x = val - 8;
                sprites[obj].SetX(x);
                // printf("\tset sprite (x%.4X) x from %d to %d | check: %s\n",address,oldx,x, (sprites[obj].GetX() == x) ? "true" : "false");
            }
                break;
            case 2: {
                sprites[obj].SetTile(val);
                // printf("\tset sprite (x%.4X) x from %.4X to %.4X | check: %s\n",address,tile,val, (sprites[obj].GetX() == val) ? "true" : "false");
            }
                break;
            case 3: {
                sprites[obj].SetPalette ((val & 0x10) != 0 ? 1 : 0);
                sprites[obj].SetFlipX   ((val & 0x20) != 0);
                sprites[obj].SetFlipY   ((val & 0x40) != 0);
                sprites[obj].SetPriority((val & 0x80) == 0);
            }
                break;
        }
    }
}

void MMU::CheckMemory() {
    if (Read(0xFF05) != 0x00) { std::cout << "failed on FF0F" << std::endl; exit(1); } // TIMA
    if (Read(0xFF06) != 0x00) { std::cout << "failed on FF0F" << std::endl; exit(1); } // TMA
    if (Read(0xFF07) != 0x00) { std::cout << "failed on FF07" << std::endl; exit(1); } // TAC
    // if (Read(0xFF10) != 0x80) { std::cout << "failed on FF10" << std::endl; exit(1); } // NR10
    // if (Read(0xFF11) != 0xBF) { std::cout << "failed on FF11" << std::endl; exit(1); } // NR11
    // if (Read(0xFF12) != 0xF3) { std::cout << "failed on FF12" << std::endl; exit(1); } // NR12
    // if (Read(0xFF14) != 0xBF) { std::cout << "failed on FF14" << std::endl; exit(1); } // NR14
    // if (Read(0xFF16) != 0x3F) { std::cout << "failed on FF16" << std::endl; exit(1); } // NR21
    // if (Read(0xFF17) != 0x00) { std::cout << "failed on FF17" << std::endl; exit(1); } // NR22
    // if (Read(0xFF19) != 0xBF) { std::cout << "failed on FF19" << std::endl; exit(1); } // NR24
    // if (Read(0xFF1A) != 0x7F) { std::cout << "failed on FF1A" << std::endl; exit(1); } // NR30
    // if (Read(0xFF1B) != 0xFF) { std::cout << "failed on FF1B" << std::endl; exit(1); } // NR31
    // if (Read(0xFF1C) != 0x9F) { std::cout << "failed on FF1C" << std::endl; exit(1); } // NR32
    // if (Read(0xFF1E) != 0xBF) { std::cout << "failed on FF1E" << std::endl; exit(1); } // NR33
    // if (Read(0xFF20) != 0xFF) { std::cout << "failed on FF20" << std::endl; exit(1); } // NR41
    // if (Read(0xFF21) != 0x00) { std::cout << "failed on FF21" << std::endl; exit(1); } // NR42
    // if (Read(0xFF22) != 0x00) { std::cout << "failed on FF22" << std::endl; exit(1); } // NR43
    // if (Read(0xFF23) != 0xBF) { std::cout << "failed on FF23" << std::endl; exit(1); } // NR30
    // if (Read(0xFF24) != 0x77) { std::cout << "failed on FF24" << std::endl; exit(1); } // NR50
    // if (Read(0xFF25) != 0xF3) { std::cout << "failed on FF25" << std::endl; exit(1); } // NR51
    // if (Read(0xFF26) != 0xF1) { std::cout << "failed on FF26" << std::endl; exit(1); } // NR52
    if (Read(0xFF40) != 0x91) { std::cout << "failed on FF40" << std::endl; exit(1); } // LCDC
    if (Read(0xFF42) != 0x00) { std::cout << "failed on FF42" << std::endl; exit(1); } // SCY
    if (Read(0xFF43) != 0x00) { std::cout << "failed on FF43" << std::endl; exit(1); } // SCX
    if (Read(0xFF45) != 0x00) { std::cout << "failed on FF45" << std::endl; exit(1); } // LYC
    if (Read(0xFF47) != 0xFC) { std::cout << "failed on FF47" << std::endl; exit(1); } // BGP
    if (Read(0xFF4A) != 0x00) { std::cout << "failed on FF4A" << std::endl; printf("%.2X", Read(0xFF4A)); exit(1); } // WY
    if (Read(0xFF4B) != 0x00) { std::cout << "failed on FF4B" << std::endl; printf("%.2X", Read(0xFF4B)); exit(1); } // WX
    // if (Read(0xFFFF) != 0x00) { std::cout << "failed on FFFF" << std::endl; printf("%.2X", Read(0xFFFF)); exit(1); } // IE
}
