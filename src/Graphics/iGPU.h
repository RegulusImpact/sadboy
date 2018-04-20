#ifndef iGPU_H
#define iGPU_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

class iGPU {
protected:
    // internal reference variables
    static const uint8_t MAX_Y = 144;
    static const uint8_t MAX_X = 160;

    enum GPU_MODE {
        OAM = 0b10,
        VRAM = 0b11,
        HBLANK = 0b00,
        VBLANK = 0b01
    };

    // internal variables
    uint32_t clocks;
    uint8_t framebuffer[(MAX_X*MAX_Y)];

    // local variables used to sync to and from memory
    // control
    static const uint16_t CONTROL_ADDRESS = 0xFF40;
    uint8_t control;
    // control - breakdown bits
    // bit 7 - lcd control operation
        // 0: stop completely (no picture)
        // 1: operation
        bool lcdOperation;
    // bit 6 - window tilemap display select
        // 0: 9800-9bff
        // 1: 9c00-9fff
        bool windowTilemap;
    // bit 5 - window display
        // 0: off
        // 1: on
        bool windowDisplay;
    // bit 4 - bg + window tile data select
        // 0: 8800-97ff
        // 1: 8000-9fff
        bool bgTile;
    // bit 3 - bg tilemap displayselect
        // 0: 9800-9bff
        // 1: 9c00-9fff
        bool bgMap;
    // bit 2 - obj (sprite) size -- width x height
        // 0: 8x8
        // 1: 8x16
        bool spriteSize;
    // bit 1 - obj (sprite) display
        // 0: off
        // 1: on
        bool spriteDisplay;
    // bit 0 bg + window display
        // 0: off
        // 1: on
        bool bgDisplay;

    // lcdc status
    static const uint16_t STATUS_ADDRESS = 0xFF41;
    uint8_t lcdStat;
    // bit 7 - unused
    // bit 6 - selectable lyc == ly
    bool useLYC;
    // bit 5 - mode 10 interrupt
    // bit 4 - mode 01 interrupt
    // bit 3 - mode 00 interrupt
        // 0: lyc != ly
        // 1: lyc == ly
    // bit 2 -lyc == ly coincidence
    bool coincidence;
    // bit 1 - 0 -mode flag
    GPU_MODE mode;

    // scroll y
    static const uint16_t SCROLLY_ADDRESS = 0xFF42;
    uint8_t scrollY;

    // scroll x
    static const uint16_t SCROLLX_ADDRESS = 0xFF43;
    uint8_t scrollX;

    // LY (scanline)
    static const uint16_t SCANLINE_ADDRESS = 0xFF44;
    uint8_t scanline;

    // LYC (ly compare)
    static const uint16_t LYC_ADDRESS = 0xFF45;
    uint8_t lyc;

    // window y
    static const uint16_t WINDOWY_ADDRESS = 0xFF4A;
    uint8_t windowY;

    // window x
    static const uint16_t WINDOWX_ADDRESS = 0xFF4B;
    uint8_t windowX;

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
    virtual std::uint8_t GetLYC()=0;
    virtual std::uint8_t GetWindowX()=0;
    virtual std::uint8_t GetWindowY()=0;

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
    virtual void SetLYC(std::uint8_t val)=0;
    virtual void SetWindowX(std::uint8_t val)=0;
    virtual void SetWindowY(std::uint8_t val)=0;

    // drawing
    virtual void RenderFrame()=0;
    virtual void Draw(uint8_t color, uint8_t y, uint8_t x)=0;
    virtual void DumpTileset()=0;
    virtual void DumpSprites()=0;
    virtual void DumpTiles()=0;
    //void SetClocks(std::uint8_t val)      { clocks = val; }

    // Rendering
    virtual void Step(uint32_t clockStep)=0;
    virtual void Hblank()=0;
    virtual void RenderScanline()=0;
    //virtual void UpdateTile(std::uint16_t addr, std::uint8_t value);
};

#endif
