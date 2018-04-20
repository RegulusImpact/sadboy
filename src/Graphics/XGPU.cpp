// XGPU.cpp

#include "XGPU.h"

void XGPU::init_x() {
    // setup up the display variables
    display = XOpenDisplay((char*)0);

    if (display == NULL) {
        std::cerr << "X could not open display. Make sure the DISPLAY environment variable is configured." << std::endl;
        std::cerr << "E.g. DISPLAY:0" << std::endl;
        exit(1);
    }

    screen = XDefaultScreen(display);

    black = BlackPixel(display, screen);
    white = WhitePixel(display, screen);

    // create the window
    window = XCreateSimpleWindow(
        display,
        DefaultRootWindow(display),
        0,
        0,
        (windowScalar * MAX_X), // 200 wide
        (windowScalar * MAX_Y), // 300 down
        5,
        white, // foreground white
        black // background black
    );

    // set window properties
    XSetStandardProperties(
        display,
        window,
        "Sadboy", // Title maximized
        "Emulator", // Title minimized
        None,
        NULL,
        0,
        NULL
    );

    // set inputs
    XSelectInput(display, window, ExposureMask|ButtonPressMask|KeyPressMask);

    // create graphics context
    gc = XCreateGC(display, window, 0, 0);

    // set the fore/back ground colors currently in use in the window
    XSetForeground(display, gc, white);
    XSetBackground(display, gc, white);

    // clear the window and bring to foreground
    XClearWindow(display, window);
    XMapRaised(display, window);

    XDrawString(display, window, gc, 5, 5,"Sadboy world!", strlen("Sadboy world!"));
}

void XGPU::close_x() {
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void XGPU::init_palette() {
    Visual* visual = DefaultVisual(display, screen);

    cmap = XCreateColormap(
        display,
        RootWindow(display, screen),
        visual,
        AllocNone
    );

    for (int ii = 0; ii < 4; ii++) {
        uint8_t mmuColor = mmu->palette[ii];
        Status rc;

        switch (mmuColor) {
            XColor xc;
            case 0:
            {
                rc = XAllocNamedColor(display, cmap, "Gray100", &palette[ii], &xc);
                // rc = XAllocNamedColor(display, cmap, "Gray100", &palette[ii], &xc);
            }
                break;
            case 1:
            {
                rc = XAllocNamedColor(display, cmap, "Gray85", &palette[ii], &xc);
                // rc = XAllocNamedColor(display, cmap, "Gray85", &palette[ii], &xc);
            }
                break;
            case 2:
            {
                rc = XAllocNamedColor(display, cmap, "Gray55", &palette[ii], &xc);
                // rc = XAllocNamedColor(display, cmap, "Gray55", &palette[ii], &xc);
            }
                break;
            case 3:
            {
                rc = XAllocNamedColor(display, cmap, "Gray0", &palette[ii], &xc);
                // rc = XAllocNamedColor(display, cmap, "Gray0", &palette[ii], &xc);
            }
                break;
        }

        if (rc == 0) {
            printf("Color Alloc [%d] failed.\n", ii);
        }
    }

    XSetWindowColormap(display, window, cmap);
}

XGPU::XGPU(MMU* m, uint8_t ws = 1){
    mmu = m;
    windowScalar = ws;
    init_x();
    init_palette();
}

XGPU::~XGPU() {
    close_x();
    delete mmu;
}
// Getters
uint8_t XGPU::GetControl()   { return mmu->Read(CONTROL_ADDRESS); }
uint8_t XGPU::GetLCDStat()   { return mmu->Read(STATUS_ADDRESS); }
uint8_t XGPU::GetScrollY()   { return mmu->Read(SCROLLY_ADDRESS); }
uint8_t XGPU::GetScrollX()   { return mmu->Read(SCROLLX_ADDRESS); }
uint8_t XGPU::GetScanline()  { return mmu->Read(SCANLINE_ADDRESS); }
uint8_t XGPU::GetLYC()       { return mmu->Read(LYC_ADDRESS); }
uint8_t XGPU::GetWindowY()   { return mmu->Read(WINDOWY_ADDRESS); }
uint8_t XGPU::GetWindowX()   { return mmu->Read(WINDOWX_ADDRESS); }

// Setters
void XGPU::SetControl(uint8_t val)  { mmu->Write(CONTROL_ADDRESS, val); }
void XGPU::SetLCDStat(uint8_t val)  { mmu->Write(STATUS_ADDRESS, val); }
void XGPU::SetScrollY(uint8_t val)  { mmu->Write(SCROLLY_ADDRESS, val); }
void XGPU::SetScrollX(uint8_t val)  { mmu->Write(SCROLLX_ADDRESS, val); }
void XGPU::SetScanline(uint8_t val) { mmu->Write(SCANLINE_ADDRESS, val); }
void XGPU::SetLYC(uint8_t val)      { mmu->Write(LYC_ADDRESS, val); }
void XGPU::SetWindowY(uint8_t val)  { mmu->Write(WINDOWY_ADDRESS, val); }
void XGPU::SetWindowX(uint8_t val)  { mmu->Write(WINDOWX_ADDRESS, val); }

// Special Getters
void XGPU::Sync() {
    // control
    control = GetControl();
        lcdOperation = (control & Utils::BIT_7) != 0;
        windowTilemap = (control & Utils::BIT_6) != 0;
        windowDisplay = (control & Utils::BIT_5) != 0;
        bgTile = (control & Utils::BIT_4) == 0; // 0 is signed mode
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

    // Window Y
    windowY = GetWindowY();

    // Window X
    windowX = GetWindowX();
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
                    //XFlush(display);
                }
            }
        }
            break;
    }

    SyncMemory();
}

void XGPU::Hblank() {}

void XGPU::RenderScanline() {
    if (bgDisplay) {
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
        size_t offset = MAX_X * scanline;

        if (bgTile && (tile < 128)) tile += 256;

        for (int ii = 0; ii < MAX_X; ii++) {
            color = mmu->tiles[tile][y][x];
            // color = mmu->bgp[color];
            framebuffer[offset] = mmu->bgp[color];
            offset++;

            x++;
            if (x == 8) {
                x = 0;
                lineOffset = (lineOffset + 1) & 31;
                tile = mmu->Read((bgmapOffset + lineOffset));
                if (bgTile && (tile < 128)) tile += 256;
            }
        }
    }

    if (windowDisplay) {
        printf("WINDOW DISPLAY\n");
        int qq;
        std::cin >> qq;
    }

    if (spriteDisplay) {
        uint16_t spriteStart = 0xFE00;
        size_t offset = MAX_X * scanline;
        uint8_t spriteLineCount = 0;
        for (uint8_t sprite = 0; sprite < 40; sprite++) {
            if(spriteLineCount >= 10) {
                break;
            }
            bool rendered = false;
            uint8_t index = sprite * 4;

            uint16_t spriteIndex = spriteStart + index;
            uint8_t y = mmu->Read(spriteIndex) - 16;
            uint8_t x = mmu->Read(spriteIndex + 1) - 8;
            uint8_t tileIndex = mmu->Read(spriteIndex + 2);
            uint8_t attribs = mmu->Read(spriteIndex + 3);

            bool priority = (attribs & Utils::BIT_7) != 0;
            bool flipY = (attribs & Utils::BIT_6) != 0;
            bool flipX = (attribs & Utils::BIT_5) != 0;
            bool useAltPalette = (attribs & Utils::BIT_4) != 0;

            uint8_t size = spriteSize ? 16 : 8;

            if (scanline >= y && scanline < (y + size)) {
                uint8_t currentline = 2;

                currentline *= (flipY ? (y + size - scanline) : (scanline - y));

                uint16_t address = 0xA000 + (tileIndex * 16) + currentline;

                // the bytes are in order lo to high
                uint8_t lo = mmu->Read((address));
                uint8_t hi = mmu->Read((address + 1));

                for (int8_t t = 0; t < 8; x++) {
                    uint8_t color_bit = flipX ? (uint8_t)t : (uint8_t)(7 - t);

                    uint8_t pal = useAltPalette ? 1 : 0;

                    uint8_t x_pixel = (7 - t);
                    uint8_t finalPixel = x + x_pixel;

                    if (finalPixel > MAX_X) {
                        break;
                    }

                    uint8_t p0 = ((hi >> color_bit) & 2) | ((lo >> (color_bit+1)) & 1);

                    uint8_t color = mmu->obp[pal][p0];

                    if (color != 0) {
                        if (priority || framebuffer[offset+finalPixel] == 0) {
                            framebuffer[offset+finalPixel] = color;
                            rendered = true;
                        }
                    }
                }

                if (rendered) {
                    spriteLineCount++;
                }
            }
        }
    }
}

void XGPU::RenderFrame() {
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            size_t offset = (MAX_X * y) + x;
            uint8_t color = framebuffer[offset];
            Draw(color, y, x);
        }
    }
}

void XGPU::Draw(uint8_t color, uint8_t y, uint8_t x) {
    // printf("color: %d\n", color);
    Draw(palette[color], y, x);
}

void XGPU::Draw(XColor color, uint8_t y, uint8_t x) {
    XSetForeground(display, gc, color.pixel);
    XFillRectangle(
        display,
        window,
        gc,
        (x*windowScalar), // x
        (y*windowScalar), // y
        (1*windowScalar), // width
        (1*windowScalar) // height
    );
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

void XGPU::DumpSprites() {
    uint16_t tileRow = 3;
    uint16_t tileCol = 0;

	for (uint16_t t = 0; t < 40; t++) {
	    if (15 < tileCol) {
	        tileRow++;
	        tileCol = 0;
	    }

        uint8_t spriteIndex = mmu->sprites[t][2];
        uint8_t spriteData = mmu->sprites[t][3];

        // meta data broken out
        uint8_t obp   = ((spriteData & Utils::BIT_4) != 0) ? 1 : 0;

	    for (uint8_t y = 0; y < 8; y++) {
    	    for (uint8_t x = 0; x < 8; x++) {
    	        uint8_t p = mmu->tiles[spriteIndex][y][x];
                Draw(mmu->obp[obp][p], (tileRow*9)+y, (tileCol*10)+x);
    	    }
    	}

    	tileCol++;
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
    // pokemon sprite test
    // FF 00 7E FF      85 81 89 83     93 85 A5 8B    C9 97 7E FF
    // mmu->Write(vramStart, (uint8_t)0xFF);
    // mmu->Write(vramStart+1, (uint8_t)0x00);
    // mmu->Write(vramStart+2, (uint8_t)0x7E);
    // mmu->Write(vramStart+3, (uint8_t)0xFF);

    // mmu->Write(vramStart+4, (uint8_t)0x85);
    // mmu->Write(vramStart+5, (uint8_t)0x81);
    // mmu->Write(vramStart+6, (uint8_t)0x89);
    // mmu->Write(vramStart+7, (uint8_t)0x83);

    // mmu->Write(vramStart+8, (uint8_t)0x93);
    // mmu->Write(vramStart+9, (uint8_t)0x85);
    // mmu->Write(vramStart+10, (uint8_t)0xA5);
    // mmu->Write(vramStart+11, (uint8_t)0x8B);

    // mmu->Write(vramStart+12, (uint8_t)0xC9);
    // mmu->Write(vramStart+13, (uint8_t)0x97);
    // mmu->Write(vramStart+14, (uint8_t)0x7E);
    // mmu->Write(vramStart+15, (uint8_t)0xFF);

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
