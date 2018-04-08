// XGPU.h
#ifndef XGPU_H
#define XGPU_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

// X11 Support
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

// Interfaces
#include "iGPU.h"

// Other Classes
#include "../MMU.h"

// X11 programming reference
// http://math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html

class XGPU: public iGPU {

private:

    Display* display;
    int screen;
    Window window;
    GC gc;
    Colormap cmap;
    uint32_t black, white;

    XColor palette[4];
    uint8_t windowScalar;

    MMU* mmu;

void init_x();
void close_x();
void init_palette();

public:
    XGPU();
    XGPU(MMU* m, uint8_t ws);
    ~XGPU();

    // Getters

    std::uint8_t GetControl();
    std::uint8_t GetLCDStat();
    std::uint8_t GetScrollY();
    std::uint8_t GetScrollX();
    std::uint8_t GetScanline();

    //std::uint8_t GetClocks()      { return clocks; }

    // Setters


    void SetControl(std::uint8_t val);
    void SetLCDStat(std::uint8_t val);
    void SetScrollY(std::uint8_t val);
    void SetScrollX(std::uint8_t val);
    void SetScanline(std::uint8_t val);
    uint8_t IncrementScanline();
    void ResetScanline();

    void Draw(uint8_t color, uint8_t y, uint8_t x);
    void Draw(XColor color, uint8_t y, uint8_t x);
    void DumpTiles();
    void DumpTileset();
    //void SetClocks(std::uint8_t val)      { clocks = val; }

    // Rendering
    void Step(uint32_t clockStep);
    void Hblank();
    void RenderScanline();
    //void UpdateTile(std::uint16_t addr, std::uint8_t value);

};

#endif
