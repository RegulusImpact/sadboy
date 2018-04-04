// TextGPU.cpp
#include "TextGPU.h"


TextGPU::TextGPU(void): TextGPU(new MMU(new Cartridge("cart/bgbtest.gb"))) { }

TextGPU::TextGPU(MMU* m) {
    mmu = m;
    setlocale(LC_ALL, "");
    initscr(); // initialize
    noecho(); // don't echo characters
    nonl(); // no /n
    curs_set(0); // no cursor
    cbreak(); // per character input, no waiting for /n
    keypad(stdscr, true); // enable keypad / special functions keys (arrow)
    //nodelay(stdscr, true); // getch(); is non-blocking
    //wresize(stdscr, MAX_X, MAX_Y); // resize window
    scrollok(stdscr, true);
    idlok(stdscr, true); // enable keypad / special functions keys (arrow)
}

TextGPU::~TextGPU() {
    endwin();
}

/* iGPU.h */
std::uint8_t TextGPU::GetControl() {
    return mmu->Read(0xFF40);
}
std::uint8_t TextGPU::GetScrollX() {
    return mmu->Read(0xFF42);
}
std::uint8_t TextGPU::GetScrollY() {
    return mmu->Read(0xFF43);
}
std::uint8_t TextGPU::GetScanline() {
    return mmu->Read(0xFF44);
}

void TextGPU::SetControl(std::uint8_t val) {
    mmu->Write(0xFF40, val);
}
void TextGPU::SetScrollX(std::uint8_t val) {
    mmu->Write(0xFF42, val);
}
void TextGPU::SetScrollY(std::uint8_t val) {
    mmu->Write(0xFF43, val);
}
void TextGPU::SetScanline(std::uint8_t val) {
    mmu->Write(0xFF44, val);
}


// Rendering
void TextGPU::Step(uint32_t clockStep) {
    clocks += clockStep;
    uint8_t ln = GetScanline();
    //mvprintw(0, 35, "Clocks: %d", clocks);
    
    switch(mode) {
        case GPU_MODE::OAM:
        {
            // wait until OAM mode is finished, transition to VRAM mode
            if (clocks >= 80) {
                mode = GPU_MODE::VRAM;
                //uint8_t flags = mmu->Read(0xFF0F);
                //mmu->Write(0xFF0F, (uint8_t)(flags | 0b00001110));
                clocks = 0;
            }
        }
            break;
        case GPU_MODE::VRAM:
        {
            // wait until VRAM mode is finished, transition to HBLANK mode
            if (clocks >= 172) {
                mode = GPU_MODE::HBLANK;
                clocks = 0;
                
                //RenderScanline();
            }
        }
            break;
        case GPU_MODE::HBLANK:
        {
            if (clocks >= 204) {
                mode = GPU_MODE::HBLANK;
                uint8_t line = IncrementScanline();
                
                // VBLANK
                if (line > 143) {
                    mode = GPU_MODE::VBLANK;
                    //Refresh();
                } else {
                    mode = GPU_MODE::OAM;
                }
            }
        }
            break;
        case GPU_MODE::VBLANK:
        {
            if (clocks >= 456) {
                mode = GPU_MODE::HBLANK;
                uint8_t line = IncrementScanline();
                uint8_t flags = mmu->Read(0xFF0F);
                //mmu->Write(0xFF0F, (uint8_t)(flags & 0b00001110));
                
                if(line > 153) {
                    mode = GPU_MODE::OAM;
                    uint8_t flags = mmu->Read(0xFF0F);
                    mmu->Write(0xFF0F, (uint8_t)(flags | 0b00000001));
                    clocks = 0;
                    ResetScanline();
                    //Clear();
                }
            }
        }
            break;
    }
}
void TextGPU::Hblank() {
    
}
void TextGPU::RenderScanline() {
    // fetch gpu registers from memory
    uint8_t scy = GetScrollY();
    uint8_t scx = GetScrollX();
    uint8_t line = GetScanline();
    
    // which line of tiles to use in which map
    uint16_t bgmapOffset = bgmap ? 0x1C00 : 0x1800;
    bgmapOffset += ((line + scy) & 0xFF) >> 3;
    
    // which tile
    uint8_t lineoffset = scx >> 3;
    
    // which line of pixels in tile
    uint8_t y = line + scy & 7;
    
    // where in tile line to start
    uint8_t x = scx & 7;
    
    // where to render on canvas
    
    uint8_t color;
    uint8_t tile = mmu->Read(bgmapOffset + lineoffset);
    
    // if the tile data set in use is #1, the indices are signed; calc the actual offset
    if (bgtile == 1 && tile < 128) tile +=256;
    std::stringstream ss;
    char c;
    for (int ii = 0; ii < MAX_Y; ii++) {
        color = mmu->palette[mmu->tiles[tile][y][x]];
        
        if (color > 192)
            c = ii % 2 == 0 ? 'O' : '0';
        else if (color > 128)
            c = ii % 2 == 0 ? 'Z' : '2';
        else if (color > 64)
            c = ii % 2 == 0 ? ';' : ':';
        else
            c = ii % 2 == 0 ? '`' : '\'';
            
            ss.put(c);
        
        x++;
        if (x == 8) {
            x = 0;
            lineoffset = (lineoffset + 1) & 31;
            tile = mmu->Read(bgmapOffset + lineoffset);
            // if the tile data set in use is #1, the indices are signed; calc the actual offset
            if (bgtile == 1 && tile < 128) tile +=256;
        }
    }
    //ss.str("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnop");
    Print(line, 5, ss.str());
    Refresh();
}

uint8_t TextGPU::IncrementScanline() {
    uint8_t line = mmu->Read(0xFF44);
    mmu->Write(0xFF44, (uint8_t)(line+1));
    return line;
}

void TextGPU::ResetScanline() {
    mmu->Write(0xFF44, (uint8_t)0x00);
}

void TextGPU::Draw(uint8_t color, uint8_t y, uint8_t x) {
    char c;
    if (color == 3)
        c = '3';
    else if (color == 2)
        c = '2';
    else if (color == 1)
        c = '1';
    else
        c = '_';
    
    mvprintw(y, x, "%c", c);
}

void TextGPU::DumpTileset() {
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
        
        if (tileCounter == 11) {
            tileCounter = 0;
            tileRowOffset++;
        }
        
        if (tileRowOffset == 4) {
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
    
    refresh();
    getch();
}

/* TextGPU */
// NCURSES
void TextGPU::Print(std::uint32_t y, std::uint32_t x, std::string s) {
    Print(y,x,s.c_str());
}

void TextGPU::Print(std::uint32_t y, std::uint32_t x, char* s) {
    mvprintw(y,x,s);
}

void TextGPU::Print(std::uint32_t y, std::uint32_t x, const char* s) {
    mvprintw(y,x,s);
}

void TextGPU::Refresh() {
    refresh();
}

void TextGPU::Clear() {
    clear();
}

void TextGPU::GetMaxYX(std::uint32_t& y, std::uint32_t& x) {
    getmaxyx(stdscr, y, x);
}