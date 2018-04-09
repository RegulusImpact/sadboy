#ifndef iGPU_H
#define iGPU_H

#define MAX_Y 144
#define MAX_X 160

// macros from cinoop gpu.h
// i don't even know if "macros" is the right term for these
#define GPU_CONTROL_BGENABLE (1 << 0)
#define GPU_CONTROL_SPRITEENABLE (1 << 1)
#define GPU_CONTROL_SPRITEVDOUBLE (1 << 2)
#define GPU_CONTROL_TILEMAP (1 << 3)
#define GPU_CONTROL_TILESET (1 << 4)
#define GPU_CONTROL_WINDOWENABLE (1 << 5)
#define GPU_CONTROL_WINDOWTILEMAP (1 << 6)
#define GPU_CONTROL_DISPLAYENABLE (1 << 7)

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

#include "../MMU.h"

class iGPU {
protected:
    enum GPU_MODE {
        OAM = 0b10,
        VRAM = 0b11,
        HBLANK = 0b00,
        VBLANK = 0b01
    };

    GPU_MODE mode = GPU_MODE::OAM;
    std::uint32_t clocks;
    uint8_t framebuffer[(MAX_X*MAX_Y)];

public:
    iGPU(){}
    virtual ~iGPU(){}
    // Getters
    /*
    std::uint8_t GetControl()   { return mmu->Read(0xFF40); }
    std::uint8_t GetLCDStat()   { return mmu->Read(0xFF41); }
    std::uint8_t GetScrollX()   { return mmu->Read(0xFF42); }
    std::uint8_t GetScrollY()   { return mmu->Read(0xFF43); }
    std::uint8_t GetScanline()  { return mmu->Read(0xFF44); }
    */
    virtual std::uint8_t GetControl()=0;
    virtual std::uint8_t GetLCDStat()=0;
    virtual std::uint8_t GetScrollX()=0;
    virtual std::uint8_t GetScrollY()=0;
    virtual std::uint8_t GetScanline()=0;

    //std::uint8_t GetClocks()      { return clocks; }

    // Setters
    /*
    void SetControl(std::uint8_t val)   { mmu->Write(0xFF40, val); }
    void SetLCDStat(std::uint8_t val)   { mmu->Write(0xFF41, val); }
    void SetScrollX(std::uint8_t val)   { mmu->Write(0xFF42, val); }
    void SetScrollY(std::uint8_t val)   { mmu->Write(0xFF43, val); }
    void SetScanline(std::uint8_t val)  { mmu->Write(0xFF44, val); }
    */

    virtual void SetControl(std::uint8_t val)=0;
    virtual void SetLCDStat(std::uint8_t val)=0;
    virtual void SetScrollX(std::uint8_t val)=0;
    virtual void SetScrollY(std::uint8_t val)=0;
    virtual void SetScanline(std::uint8_t val)=0;
    virtual uint8_t IncrementScanline()=0;
    virtual void ResetScanline()=0;

    virtual void RenderFrame()=0;
    virtual void Draw(uint8_t color, uint8_t y, uint8_t x)=0;
    virtual void DumpTileset()=0;
    virtual void DumpTiles()=0;
    //void SetClocks(std::uint8_t val)      { clocks = val; }

    // Rendering
    virtual void Step(uint32_t clockStep)=0;
    virtual void Hblank()=0;
    virtual void RenderScanline()=0;
    //virtual void UpdateTile(std::uint16_t addr, std::uint8_t value);
};

#endif
