// Joypad.cpp
// X11 Support
#include "Joypad.h"

#include <bitset>

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

    // if bit 4 is 0, we're in dpad mode,
    // else just assume we're in button mode?
    dpadMode = (joypad & Utils::BIT_4) == 0;
    p13 = false; // bit 3 - Sta or down | if pressed, turn to 0
    p12 = false; // bit 2 - Sel or up | if pressed, turn to 0z
    p11 = false; // bit 1 - B   or left | if pressed, turn to 0
    p10 = false; // bit 0 - A   or right | if pressed, turn to 0
}

// sync values here into memory
void Joypad::syncMemory() {
    joypad =  0b11001111;
    if (dpadMode) {
        joypad |= Utils::BIT_4;
    } else {
        joypad |= Utils::BIT_5;
    }

    if (p13){
        joypad &= ~Utils::BIT_3;
    }

    if (p12) {
        joypad &= ~Utils::BIT_2;
    }

    if (p11) {
        joypad &= ~Utils::BIT_1;
    }

    if (p10) {
        joypad &= ~Utils::BIT_0;
    }

    mmu->Write(JOYPAD_ADDRESS, joypad);
    if (joypad != 0xFF && joypad != 0xDF && joypad != 0xEF && joypad != 0xF1) {
        std::bitset<8> bs(mmu->Read(JOYPAD_ADDRESS));
        std::cout << "FF00: " << bs << std::endl;
    }
}

uint16_t Joypad::CheckKeyInput() {
    syncLocal();
    uint16_t keycode = dm->GetKeyInput();
    bool raiseInterrupt = false;

    switch(keycode) {
        // pin 13
        case KEYS::START: {
            p13 = true && !dpadMode; // if dpadmode true, then false || true else false || false
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        case KEYS::DOWN: {
            p13 = true && dpadMode; // if dpadmode true, then false || false else false || true
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        // pin 12
        case KEYS::SELECT: {
            p12 = true && !dpadMode; // if dpadmode true, then false || true else false || false
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        case KEYS::UP: {
            p13 = true && dpadMode; // if dpadmode true, then false || false else false || true
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        // pin 11
        case KEYS::B: {
            p11 = true && !dpadMode; // if dpadmode true, then false || true else false || false
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        case KEYS::LEFT: {
            p13 = true && dpadMode; // if dpadmode true, then false || false else false || true
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        // pin 10
        case KEYS::A: {
            p10 = true && !dpadMode; // if dpadmode true, then false || true else false || false
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        case KEYS::RIGHT: {
            p13 = true && dpadMode; // if dpadmode true, then false || false else false || true
            raiseInterrupt = true;
            printf("keypressed: 0x%.2X\n", keycode);
        }
            break;
        default: {
            if (keycode != 0x00) {
                printf("Unknown keypressed: 0x%.2X\n", keycode);
            }
        }
            break;
    }

    // if one of these is false, we had a button press
    // so we negate to get true
    // then we raise the interrupt
    syncMemory();
    if (raiseInterrupt) {
        joypadInterrupt();
    }

    return keycode;
}
