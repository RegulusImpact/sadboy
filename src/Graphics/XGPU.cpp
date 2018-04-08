// XGPU.cpp

#include "XGPU.h"

void XGPU::init_x() {
    uint32_t black, white;

    // setup up the display variables
    display = XOpenDisplay((char*)0);
    screen = DefaultScreen(display);
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
    XSetBackground(display, gc, white);
    XSetForeground(display, gc, black);

    // clear the window and bring to foreground
    XClearWindow(display, window);
    XMapRaised(display, window);
}

void XGPU::close_x() {
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void XGPU::init_palette() {
    for (int ii = 0; ii < 4; ii++) {
        uint8_t mmuColor = (~mmu->palette[ii]) & 0xFF;
        palette[ii].red = mmuColor;
        palette[ii].green = mmuColor;
        palette[ii].blue = mmuColor;
        palette[ii].flags = DoRed | DoGreen | DoBlue;
    }

    cmap = XCreateColormap(
        display,
        RootWindow(display, screen),
        DefaultVisual(display, screen),
        AllocAll
    );

    XStoreColors(display, cmap, palette, 4);
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
void XGPU::Step(uint32_t clockStep) {}
void XGPU::Hblank() {}
void XGPU::RenderScanline() {}

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
void XGPU::DumpTileset() {}
