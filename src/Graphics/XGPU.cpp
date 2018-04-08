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
        uint8_t mmuColor = mmu->bgp[ii];
        Status rc;

        switch (mmuColor) {
            XColor xc;
            case 0:
            {
                rc = XAllocNamedColor(display, cmap, "Gray100", &palette[ii], &xc);
            }
                break;
            case 1:
            {
                rc = XAllocNamedColor(display, cmap, "Gray85", &palette[ii], &xc);
            }
                break;
            case 2:
            {
                rc = XAllocNamedColor(display, cmap, "Gray55", &palette[ii], &xc);
            }
                break;
            case 3:
            {
                rc = XAllocNamedColor(display, cmap, "Gray0", &palette[ii], &xc);
            }
                break;
        }

        if (rc == 0) {
            printf("Color Alloc [%d] failed.\n", ii);
        }
    }

    XSetWindowColormap(display, window, cmap);
}

XGPU::XGPU(void): XGPU(new MMU(new Cartridge("cart/bgbtest.gb")), 1) { }

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
std::uint8_t XGPU::GetControl()   { return mmu->Read(0xFF40); }
std::uint8_t XGPU::GetScrollX()   { return mmu->Read(0xFF42); }
std::uint8_t XGPU::GetScrollY()   { return mmu->Read(0xFF43); }
std::uint8_t XGPU::GetScanline()  { return mmu->Read(0xFF44); }

// Setters
void XGPU::SetControl(std::uint8_t val)   { mmu->Write(0xFF40, val); }
void XGPU::SetScrollX(std::uint8_t val)   { mmu->Write(0xFF42, val); }
void XGPU::SetScrollY(std::uint8_t val)   { mmu->Write(0xFF43, val); }
void XGPU::SetScanline(std::uint8_t val)  { mmu->Write(0xFF44, val); }


//Scanline functions
uint8_t XGPU::IncrementScanline() {
    uint8_t scanline = mmu->Read(0xFF44);
    scanline++;
    mmu->Write(0xFF44, scanline);
    return scanline;
}

void XGPU::ResetScanline() {
    mmu->Write(0xFF44, (uint8_t)0x00);
}

// Rendering
void XGPU::Step(uint32_t clockStep) {
    clocks += clockStep;

    bgmap = ((mmu->Read(0xFF40) >> 3) & 1) == 1;
    bgtile = ((mmu->Read(0xFF40) >> 4) & 1) == 1;

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

                RenderScanline();
            }
        }
            break;
        case GPU_MODE::HBLANK: // 0
        {
            if (clocks >= 204) {
                clocks = 0;
                uint8_t scanline = IncrementScanline();

                if (scanline > 143) {
                    mode = GPU_MODE::VBLANK;
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

                uint8_t scanline = IncrementScanline();

                if (scanline > 153) {
                    mode = GPU_MODE::OAM;
                    ResetScanline();
                    XFlush(display);
                }
            }
        }
            break;
    }
}

void XGPU::Hblank() {}
void XGPU::RenderScanline() {
    // fetch gpu registers
    uint8_t scy = GetScrollY();
    uint8_t scx = GetScrollX();
    uint8_t line = GetScanline();

    // which line of tiles to use in which map
    uint16_t bgmapOffset = bgmap ? 0x1C00 : 0x1800;
    bgmapOffset += ((line + scy) & 0xFF) >> 3;

    // which tile
    uint8_t lineOffset = scx >> 3;

    // which line of pixels in the tile
    uint8_t y = (line + scy) & 7;

    // which in the tileline to start
    uint8_t x = scx & 7;

    // where to render on canvas
    uint8_t color;
    uint8_t tile = mmu->Read(bgmapOffset + lineOffset);

    if (bgtile && tile < 128) tile += 256;

    for (int ii = 0; ii < MAX_X; ii++) {
        color = mmu->bgp[mmu->tiles[tile][y][x]];

        Draw(color, line, ii);

        x++;
        if (x == 8) {
            x = 0;
            lineOffset = (lineOffset + 1) & 31;
            tile = mmu->Read(bgmapOffset + lineOffset);
            if (bgtile && tile < 128) tile += 256;
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
    uint16_t tileRow = 0;
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

        Draw(p0, (tileRowOffset*9)+tileRow, (tileCounter*10)+0);
        Draw(p1, (tileRowOffset*9)+tileRow, (tileCounter*10)+1);
        Draw(p2, (tileRowOffset*9)+tileRow, (tileCounter*10)+2);
        Draw(p3, (tileRowOffset*9)+tileRow, (tileCounter*10)+3);
        Draw(p4, (tileRowOffset*9)+tileRow, (tileCounter*10)+4);
        Draw(p5, (tileRowOffset*9)+tileRow, (tileCounter*10)+5);
        Draw(p6, (tileRowOffset*9)+tileRow, (tileCounter*10)+6);
        Draw(p7, (tileRowOffset*9)+tileRow, (tileCounter*10)+7);

        // we've read two bytes for a row, increment
        tileRow++;
    }
}
