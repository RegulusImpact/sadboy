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

    XColor palette[4];
    uint8_t windowScalar;

    MMU* mmu;

void init_x();
void close_x();
void init_palette();

public:
      XGPU(MMU* m, uint8_t ws);
      ~XGPU();

      // Getters
      /*
      std::uint8_t GetControl()   { return mmu->Read(0xFF40); }
      std::uint8_t GetScrollX()   { return mmu->Read(0xFF42); }
      std::uint8_t GetScrollY()   { return mmu->Read(0xFF43); }
      std::uint8_t GetScanline()  { return mmu->Read(0xFF44); }
      */
      std::uint8_t GetControl();
      std::uint8_t GetScrollX();
      std::uint8_t GetScrollY();
      std::uint8_t GetScanline();

      //std::uint8_t GetClocks()      { return clocks; }

      // Setters
      /*
      void SetControl(std::uint8_t val)   { mmu->Write(0xFF40, val); }
      void SetScrollX(std::uint8_t val)   { mmu->Write(0xFF42, val); }
      void SetScrollY(std::uint8_t val)   { mmu->Write(0xFF43, val); }
      void SetScanline(std::uint8_t val)  { mmu->Write(0xFF44, val); }
      */

      void SetControl(std::uint8_t val);
      void SetScrollX(std::uint8_t val);
      void SetScrollY(std::uint8_t val);
      void SetScanline(std::uint8_t val);
      uint8_t IncrementScanline();
      void ResetScanline();

      void Draw(XColor color, uint8_t y, uint8_t x);
      void DumpTileset();
      //void SetClocks(std::uint8_t val)      { clocks = val; }

      // Rendering
      void Step(uint32_t clockStep);
      void Hblank();
      void RenderScanline();
      //void UpdateTile(std::uint16_t addr, std::uint8_t value);

};

#endif
