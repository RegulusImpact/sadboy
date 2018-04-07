// #ifndef textGPU_H
// #define textGPU_H
//
// // io
// #include <iostream>
// // uintX_t
// #include <cstdint>
// // size_t
// #include <cstddef>
// // string
// #include <string>
// #include <sstream>
// // ncurses
// #include <curses.h>
// #include <random>
//
// #include "iGPU.h"
// #include "../MMU.h"
//
// class TextGPU: public iGPU {
// private:
//     char** text;
//     MMU* mmu;
//
// public:
//     TextGPU();
//     TextGPU(MMU* mmu);
//     ~TextGPU();
//
//     void Print(std::uint32_t y, std::uint32_t x, std::string s);
//     void Print(std::uint32_t y, std::uint32_t x, const char* s);
//     void Print(std::uint32_t y, std::uint32_t x, char* s);
//     void Refresh();
//     void Clear();
//
//     void GetMaxYX(uint32_t& y, uint32_t& x);
//
//     /* iGPU.h */
//
//     std::uint8_t GetControl();
//     std::uint8_t GetScrollX();
//     std::uint8_t GetScrollY();
//     std::uint8_t GetScanline();
//     void SetControl(std::uint8_t val);
//     void SetScrollX(std::uint8_t val);
//     void SetScrollY(std::uint8_t val);
//     void SetScanline(std::uint8_t val);
//     uint8_t IncrementScanline();
//     void ResetScanline();
//     void Draw(uint8_t color, uint8_t y, uint8_t x);
//     void Step(uint32_t clockStep);
//     void Hblank();
//     void RenderScanline();
//
//     void DumpTileset();
// };
//
// #endif
