// XGPU.cpp

#include "XGPU.h"

XGPU::XGPU(MMU* m, DisplayManager* d) {
    mmu = m;
    dm = d;
}

XGPU::~XGPU() {}

// Getters
uint8_t XGPU::GetControl()   { return mmu->Read(CONTROL_ADDRESS); }
uint8_t XGPU::GetLCDStat()   { return mmu->Read(STATUS_ADDRESS); }
uint8_t XGPU::GetScrollY()   { return mmu->Read(SCROLLY_ADDRESS); }
uint8_t XGPU::GetScrollX()   { return mmu->Read(SCROLLX_ADDRESS); }
uint8_t XGPU::GetScanline()  { return mmu->Read(SCANLINE_ADDRESS); }
uint8_t XGPU::GetLYC()       { return mmu->Read(LYC_ADDRESS); }

// Setters
void XGPU::SetControl(uint8_t val)   { mmu->Write(CONTROL_ADDRESS, val); }
void XGPU::SetLCDStat(uint8_t val)   { mmu->Write(STATUS_ADDRESS, val); }
void XGPU::SetScrollY(uint8_t val)   { mmu->Write(SCROLLY_ADDRESS, val); }
void XGPU::SetScrollX(uint8_t val)   { mmu->Write(SCROLLX_ADDRESS, val); }
void XGPU::SetScanline(uint8_t val)  { mmu->Write(SCANLINE_ADDRESS, val); }
void XGPU::SetLYC(uint8_t val)       { mmu->Write(LYC_ADDRESS, val); }

// Special Getters
void XGPU::Sync() {
    // control
    control = GetControl();
        lcdOperation = (control & Utils::BIT_7) != 0;
        windowTilemap = (control & Utils::BIT_6) != 0;
        windowDisplay = (control & Utils::BIT_5) != 0;
        bgTile = (control & Utils::BIT_4) == 0;
        bgMap = (control & Utils::BIT_3) != 0;
        spriteSize = (control & Utils::BIT_2) != 0;
        spriteDisplay = (control & Utils::BIT_1) != 0;
        bgDisplay = (control & Utils::BIT_0) != 0;

    // lcdc stats
    lcdStat = GetLCDStat();
        useLYC = (lcdStat & Utils::BIT_6) != 0;
        coincidence = (lcdStat & Utils::BIT_2) != 0;
        // bit 1 - 0 -mode flag
        mode = (GPU_MODE)(lcdStat & (Utils::BIT_1 | Utils::BIT_0));

    // scroll y
    scrollY = GetScrollY();

    // scroll X
    scrollX = GetScrollX();

    // LY (scanline)
    scanline = GetScanline();

    // LYC
    lyc = GetLYC();
}
// Special Setters
void XGPU::SyncMemory() {
    // lcdc stats
    // clear bits; conditionally clear the coincidence
    lcdStat = lcdStat & ~(Utils::BIT_1 | Utils::BIT_0) & ~(useLYC ? Utils::BIT_2 : 0x00);
    lcdStat |= mode;
    lcdStat |= (scanline == lyc) ? Utils::BIT_2 : 0x00;

    if (scanline == lyc) {
        TriggerLCDStat(Utils::BIT_6); // trigger hblank lcdc status interrupt
    }
    SetLCDStat(lcdStat);

    // LY (scanline)
    SetScanline(scanline);
}

//Scanline functions
void XGPU::IncrementScanline() {
    scanline++;
}

void XGPU::ResetScanline() {
    scanline = 0;
}

void XGPU::TriggerVBlank() {
    uint8_t flags = mmu->Read(InterruptService::IFLAGS);
    flags |= InterruptService::vBlankBit;
    mmu->Write(InterruptService::IFLAGS, flags);
}

void XGPU::TriggerLCDStat(uint8_t statusBit) {
    uint8_t flags = mmu->Read(InterruptService::IFLAGS);
    flags |= InterruptService::lcdStatBit;
    mmu->Write(InterruptService::IFLAGS, flags);


    lcdStat &=~ Utils::BIT_6;
    lcdStat &=~ Utils::BIT_5;
    lcdStat &=~ Utils::BIT_4;
    lcdStat &=~ Utils::BIT_3;

    lcdStat |= statusBit;
    SetLCDStat(lcdStat);
}

// Rendering
void XGPU::Step(uint32_t clockStep) {
    Sync();
    clocks += clockStep;

    switch (mode) {
        case GPU_MODE::OAM: // 2
        {
            if (clocks >= 80) {
                clocks = 0;
                mode = GPU_MODE::VRAM;
            }
        }
            break;
        case GPU_MODE::VRAM: // 3
        {
            if (clocks >= 172) {
                clocks = 0;
                mode = GPU_MODE::HBLANK;
                TriggerLCDStat(Utils::BIT_3); // trigger hblank lcdc status interrupt

                RenderScanline();
            }
        }
            break;
        case GPU_MODE::HBLANK: // 0
        {
            if (clocks >= 204) {
                clocks = 0;
                IncrementScanline();

                if (scanline == 143) {
                    mode = GPU_MODE::VBLANK;
                    TriggerVBlank();
                    TriggerLCDStat(Utils::BIT_4); // trigger vblank lcdc status interrupt
                    RenderFrame();
                } else {
                    mode = GPU_MODE::OAM;
                }
            }
        }
            break;
        case GPU_MODE::VBLANK: // 1
        {
            if (clocks >= 456) {
                clocks = 0;

                IncrementScanline();

                if (scanline > 153) {
                    mode = GPU_MODE::OAM;
                    TriggerLCDStat(Utils::BIT_5); // trigger oam-rams lcdc status interrupt

                    ResetScanline();
                    // XFlush(display);
                }
            }
        }
            break;
    }

    SyncMemory();
}

void XGPU::Hblank() {}


void XGPU::renderBackground(uint8_t scanrow[Utils::MAX_X]) {
    // which line of tiles to use in which map
    uint16_t bgmapOffset = bgMap ? 0x1C00 : 0x1800;
    bgmapOffset += (((scanline + scrollY) & 0xFF) >> 3) << 5;
    bgmapOffset += 0x8000;

    // which tile
    uint8_t lineOffset = (scrollX >> 3);

    // which line of pixels in the tile
    uint8_t y = (scanline + scrollY) & 7;

    // which in the tileline to start
    uint8_t x = (scrollX & 7);

    // where to render on canvas
    uint8_t color;
    uint16_t tile = mmu->Read((bgmapOffset + lineOffset));
    size_t offset = (Utils::MAX_X * scanline);

    if (bgTile && tile < 128) tile += 256;

    for (int ii = 0; ii < Utils::MAX_X; ii++) {
        color = mmu->tiles[tile][y][x];
        // color = mmu->bgp[color];
        framebuffer[offset] = mmu->bgp[color];
        scanrow[ii] = color;
        offset++;

        x++;
        if (x == 8) {
            x = 0;
            lineOffset = (lineOffset + 1) & 31;
            tile = mmu->Read((bgmapOffset + lineOffset));
            if (bgTile && tile < 128) tile += 256;
        }
    }
}

void XGPU::renderWindows(uint8_t scanrow[Utils::MAX_X]) {

}

void XGPU::renderSprites(uint8_t scanrow[Utils::MAX_X]) {
    uint8_t size = spriteSize ? 16 : 8;
    for (size_t ii = 0; ii < 40; ii++) {
        Sprite obj = mmu->sprites[ii];

        if ((obj.GetY() <= scanline) && ((obj.GetY() + 8) > scanline)) {// possible error zone
            uint8_t pal = obj.GetPalette();

            size_t canvasOffset = (Utils::MAX_X * scanline) + obj.GetX();

            //int8_t tilerow = obj.GetFlipY() ? (7 - (scanline - obj.GetY())) : (scanline - obj.GetY());
            uint8_t* tilerow;

            if (obj.GetFlipY()) {
                tilerow = mmu->tiles[obj.GetTile()][( 7 - ( scanline - obj.GetY() ) )];
            } else {
                tilerow = mmu->tiles[obj.GetTile()][(scanline - obj.GetY())];
            }

            uint8_t color;

            for(int x = 0; x < 8; x++) {
                int8_t actualX = obj.GetFlipX() ? (7 - x) : (x);
                color = tilerow[actualX];
                if (((obj.GetX() + x) >= 0) &&
                    ((obj.GetX() + x) < Utils::MAX_X) &&
                    (color != 0) &&
                    ( (obj.GetPriority()) || (scanrow[(obj.GetX() + x)] == 0) ) // canvasoffset already includes the obj.GetX()
                ) {
                    framebuffer[canvasOffset] = mmu->obp[pal][color];
                }
                canvasOffset++;
            }
        }
    }
}

void XGPU::RenderScanline() {
    if (lcdOperation) {
        uint8_t scanrow[Utils::MAX_X];
        if (bgDisplay) {
            renderBackground(scanrow);
        }

        if (windowDisplay) {
            renderWindows(scanrow);
        }

        if (spriteDisplay) {
            renderSprites(scanrow);
        }
    }
}

void XGPU::RenderFrame() {
    for (int y = 0; y < Utils::MAX_Y; y++) {
        for (int x = 0; x < Utils::MAX_X; x++) {
            size_t offset = (Utils::MAX_X * y) + x;
            uint8_t color = framebuffer[offset];
            Draw(color, y, x);
        }
    }
}

void XGPU::Draw(uint8_t color, uint8_t y, uint8_t x) {
    dm->Draw(color, y, x);
}

void XGPU::DumpTiles() {
    uint16_t tileRow = 3;
    uint16_t tile = 0;

	for (uint16_t t = 0; t < 32; t++) {
	    if (15 < tile) {
	        tileRow++;
	        tile = 0;
	    }

	    for (uint8_t y = 0; y < 8; y++) {
    	    for (uint8_t x = 0; x < 8; x++) {
    	        uint8_t p = mmu->tiles[t][y][x];
                Draw(mmu->bgp[p], (tileRow*9)+y, (tile*10)+x);
    	    }
    	}

    	tile++;
	}
}
void XGPU::DumpTileset() {
    /*

    iterate through vram
    every 16 bytes is am 8x8 tile
    byte pairs read read low then high

    tileRow 0 | byte 0 | byte 1
    tileRow 1 | byte 2 | byte 3
    tileRow 3 | byte 4 | byte 5
    tileRow 3 | byte 6 | byte 7
    tileRow 4 | byte 8 | byte 9
    tileRow 5 | byte A | byte B
    tileRow 6 | byte C | byte D
    tileRow 7 | byte E | byte F

    Row
      byte one (hi)  |   byte two (lo)
        1234 5678    |     ABCD EFGH

    Colors:
    p0 p1 p2 p3 p4 p5 p6 p7
    1A 2B 3C 4D 5E 6F 7G 8G


    */
    uint32_t tileCounter = 0;
    uint16_t tileRow = 0;
    uint16_t tileRowOffset = 0;
    uint16_t maxVRAM = 0x2000;
    uint16_t vramStart = 0x8000;
    // FF 00 7E FF      85 81 89 83     93 85 A5 8B    C9 97 7E FF

    for (uint16_t ii = 0; ii < maxVRAM; ii += 2) {
        // ii iterates through every byte in vram
        // ii % 16 is which byte of the tile we're looking at
        // each time ii % 16 == 0, increment our tile counter
        // we want to read two bytes at a time
        if (0 == ii % 16 && 0 != ii) {
            tileCounter++;
            // we reset tileRow when tileCounter goes up
            tileRow = 0;
        }

        if (16 < tileCounter) {
            tileCounter = 0;
            tileRowOffset++;
        }

        if (tileRowOffset == 3) {
            break;
        }


        // the bytes are in order lo to high
        uint8_t lo = mmu->Read((vramStart + ii));
        uint8_t hi = mmu->Read((vramStart + ii + 1));

        // 1000 0000 >> 6 & 2 == 0000 001x & 2 == 0000 0010
        uint8_t p0 = ((hi >> 6) & 2) | ((lo >> 7) & 1);
        uint8_t p1 = ((hi >> 5) & 2) | ((lo >> 6) & 1);
        uint8_t p2 = ((hi >> 4) & 2) | ((lo >> 5) & 1);
        uint8_t p3 = ((hi >> 3) & 2) | ((lo >> 4) & 1);
        uint8_t p4 = ((hi >> 2) & 2) | ((lo >> 3) & 1);
        uint8_t p5 = ((hi >> 1) & 2) | ((lo >> 2) & 1);
        uint8_t p6 =  (hi &  2)      | ((lo >> 1) & 1);
        uint8_t p7 = ((hi << 1) & 2) |  (lo &  1);

        // mvprintw(tileRow, 50, "%d", p0);
        // mvprintw(tileRow, 51, "%d", p1);
        // mvprintw(tileRow, 52, "%d", p2);
        // mvprintw(tileRow, 53, "%d", p3);
        // mvprintw(tileRow, 54, "%d", p4);
        // mvprintw(tileRow, 55, "%d", p5);
        // mvprintw(tileRow, 56, "%d", p6);
        // mvprintw(tileRow, 57, "%d", p7);

        Draw(mmu->bgp[p0], (tileRowOffset*9)+tileRow, (tileCounter*10)+0);
        Draw(mmu->bgp[p1], (tileRowOffset*9)+tileRow, (tileCounter*10)+1);
        Draw(mmu->bgp[p2], (tileRowOffset*9)+tileRow, (tileCounter*10)+2);
        Draw(mmu->bgp[p3], (tileRowOffset*9)+tileRow, (tileCounter*10)+3);
        Draw(mmu->bgp[p4], (tileRowOffset*9)+tileRow, (tileCounter*10)+4);
        Draw(mmu->bgp[p5], (tileRowOffset*9)+tileRow, (tileCounter*10)+5);
        Draw(mmu->bgp[p6], (tileRowOffset*9)+tileRow, (tileCounter*10)+6);
        Draw(mmu->bgp[p7], (tileRowOffset*9)+tileRow, (tileCounter*10)+7);

        // we've read two bytes for a row, increment
        tileRow++;
    }
}
