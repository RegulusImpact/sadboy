// Joypad.cpp
// X11 Support
#include "Joypad.h"

Joypad::Joypad(MMU* m, DisplayManager* d) {
    mmu = m;
    dm = d;
}

void Joypad::joypadInterrupt() {
    uint8_t flags = mmu->Read(InterruptService::IFLAGS);
    flags |= InterruptService::joyPadBit;
    mmu->Write(InterruptService::IFLAGS, flags);
}

// sync values in memory to here
void Joypad::syncLocal() {
    joypad = mmu->Read(JOYPAD_ADDRESS);

    // if bit 5 is 1 AND bit 4 is 0, we're in dpad mode,
    // else just assume we're in button mode?
    dpadMode = ( ( ( joypad & Utils::BIT_5 ) != 0 ) && ( ( joypad & Utils::BIT_4) == 0 ) );
    p13 = true; // bit 3 - Sta or down | if pressed, turn to 0
    p12 = true; // bit 2 - Sel or up | if pressed, turn to 0
    p11 = true; // bit 1 - B   or left | if pressed, turn to 0
    p10 = true; // bit 0 - A   or right | if pressed, turn to 0
}

// sync values here into memory
void Joypad::syncMemory() {
    joypad = 0b11000000; // unused buts
    joypad |=  dpadMode ?   Utils::BIT_5 : 0x00;
    joypad |= !dpadMode ?   Utils::BIT_4 : 0x00;
    joypad |= !p13 ? 0x00 : Utils::BIT_3;
    joypad |= !p12 ? 0x00 : Utils::BIT_2;
    joypad |= !p11 ? 0x00 : Utils::BIT_1;
    joypad |= !p10 ? 0x00 : Utils::BIT_0;

    mmu->Write(JOYPAD_ADDRESS, joypad);
}

void Joypad::CheckKeyInput() {
    syncLocal();
    uint16_t keycode = dm->GetKeyInput();

    switch(keycode) {
        // pin 13
        case KEYS::START: case KEYS::DOWN: {
            printf("PIN 13: 0x%.2X\n", keycode);
            p13 = false;
        }
            break;
        // pin 12
        case KEYS::SELECT: case KEYS::UP: {
            printf("PIN 12: 0x%.2X\n", keycode);
            p12 = false;
        }
            break;
        // pin 11
        case KEYS::B: case KEYS::LEFT: {
            printf("PIN 11: 0x%.2X\n", keycode);
            p11 = false;
        }
            break;
        // pin 10
        case KEYS::A: case KEYS::RIGHT: {
            printf("PIN 10: 0x%.2X\n", keycode);
            p10 = false;
        }
            break;
    }

    syncMemory();
    // if one of these is false, we had a button press
    // so we negate to get true
    // then we raise the interrupt
    if (!(p13 && p12 && p11 && p10)) {
        joypadInterrupt();
    }
}
