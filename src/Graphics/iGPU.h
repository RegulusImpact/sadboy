#ifndef iGPU_H
#define iGPU_H

#define MAX_X 144
#define MAX_Y 160

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

#include "../MMU.h"

class iGPU {
protected:
    enum GPU_MODE {
        OAM,
        VRAM,
        HBLANK,
        VBLANK
    };

    GPU_MODE mode = GPU_MODE::OAM;
    std::uint32_t clocks;
    bool bgmap; // false = 0, true = 1
    bool bgtile; // false = 0, true = 1

public:
    iGPU(){}
    virtual ~iGPU(){}
    // Getters
    /*
    std::uint8_t GetControl()   { return mmu->Read(0xFF40); }
    std::uint8_t GetScrollX()   { return mmu->Read(0xFF42); }
    std::uint8_t GetScrollY()   { return mmu->Read(0xFF43); }
    std::uint8_t GetScanline()  { return mmu->Read(0xFF44); }
    */
    virtual std::uint8_t GetControl()=0;
    virtual std::uint8_t GetScrollX()=0;
    virtual std::uint8_t GetScrollY()=0;
    virtual std::uint8_t GetScanline()=0;

    //std::uint8_t GetClocks()      { return clocks; }

    // Setters
    /*
    void SetControl(std::uint8_t val)   { mmu->Write(0xFF40, val); }
    void SetScrollX(std::uint8_t val)   { mmu->Write(0xFF42, val); }
    void SetScrollY(std::uint8_t val)   { mmu->Write(0xFF43, val); }
    void SetScanline(std::uint8_t val)  { mmu->Write(0xFF44, val); }
    */

    virtual void SetControl(std::uint8_t val)=0;
    virtual void SetScrollX(std::uint8_t val)=0;
    virtual void SetScrollY(std::uint8_t val)=0;
    virtual void SetScanline(std::uint8_t val)=0;
    virtual uint8_t IncrementScanline()=0;
    virtual void ResetScanline()=0;

    virtual void Draw(uint8_t color, uint8_t y, uint8_t x)=0;
    virtual void DumpTileset()=0;
    //void SetClocks(std::uint8_t val)      { clocks = val; }

    // Rendering
    virtual void Step(uint32_t clockStep)=0;
    virtual void Hblank()=0;
    virtual void RenderScanline()=0;
    //virtual void UpdateTile(std::uint16_t addr, std::uint8_t value);
};

#endif
