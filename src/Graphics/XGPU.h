// XGPU.h
#ifndef XGPU_H
#define XGPU_H

// Interfaces
#include "iGPU.h"

// Other Classes
#include "DisplayManager.h"
#include "../MMU.h"
#include "../Utils.h"
#include "../InterruptService.h"

// X11 programming reference
// http://math.msu.su/~vvb/2course/Borisenko/CppProjects/GWindow/xintro.html

class XGPU: public iGPU {

private:
    // functionality dependent
    MMU* mmu;
    DisplayManager* dm;

void renderBackground(uint8_t scanrow[Utils::MAX_X]);
void renderWindows(uint8_t scanrow[Utils::MAX_X]);
void renderSprites(uint8_t scanrow[Utils::MAX_X]);

public:
    XGPU(MMU* m, DisplayManager* d);
    ~XGPU();

    // Getters
    std::uint8_t GetControl();
    std::uint8_t GetLCDStat();
    std::uint8_t GetScrollY();
    std::uint8_t GetScrollX();
    std::uint8_t GetScanline();
    std::uint8_t GetLYC();

    // Setters
    void SetControl(std::uint8_t val);
    void SetLCDStat(std::uint8_t val);
    void SetScrollY(std::uint8_t val);
    void SetScrollX(std::uint8_t val);
    void SetScanline(std::uint8_t val);
    void SetLYC(std::uint8_t val);

    void IncrementScanline();
    void ResetScanline();

    void TriggerVBlank();
    void TriggerLCDStat(uint8_t statusBit);

    // Special Getters - pull values from memory at the startup / start step
    void Sync();
    // Special Setters - set values back into memory at end step
    void SyncMemory();

    // Rendering
    void Step(uint32_t clockStep);
    void Hblank();
    void RenderScanline();

    void RenderFrame();
    void Draw(uint8_t color, uint8_t y, uint8_t x);
    void DumpTiles();
    void DumpTileset();

};

#endif
