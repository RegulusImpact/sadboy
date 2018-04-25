// Joypad.h
#ifndef Joypad_H
#define Joypad_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

#include "Graphics/DisplayManager.h"
#include "Utils.h"
#include "InterruptService.h"

class Joypad {
private:
    enum KEYS {
        A = 0x2F,        // ;
        B = 0x2E,        // l
        SELECT = 0x3E,   // shift
        START = 0x24,    // enter
        UP = 0x19,   // w
        LEFT = 0x26, // a
        DOWN = 0x27, // s
        RIGHT = 0x28 // d
    };

    MMU* mmu;
    DisplayManager* dm;

    // Joypad
    uint8_t joypad;
    static const uint16_t JOYPAD_ADDRESS = 0xFF00;
    bool dpadMode; // bit5 == 0, we're in button mode | bit4 == 0 we're in dpad mode
    bool p13; // bit 3 - Sta or down
    bool p12; // bit 2 - Sel or up
    bool p11; // bit 1 - B   or left
    bool p10; // bit 0 - A   or right

    void joypadInterrupt();
    void syncLocal(); // sync values in memory to here
    void syncMemory(); // sync values here into memory
public:
    Joypad(MMU* m, DisplayManager* d);

    void CheckKeyInput();
};

#endif
