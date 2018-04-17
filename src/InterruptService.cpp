// InterruptService.cpp
#include "InterruptService.h"

InterruptService::InterruptService(CPU* c, MMU* m) {
    cpu = c;
    mmu = m;
}

InterruptService::~InterruptService() {
    delete cpu;
    delete mmu;
}

void InterruptService::DisableHalt() {
    uint8_t ie = mmu->Read(IE);
    uint8_t flags = mmu->Read(IFLAGS);

    // if any interrupt enable bit and corresponding flags are set
    // regardless of the IME, disable HALT
    if (((ie & flags & vBlankBit)  > 0) ||
        ((ie & flags & lcdStatBit) > 0) ||
        ((ie & flags & timerBit)   > 0) ||
        ((ie & flags & serialBit)  > 0) ||
        ((ie & flags & joyPadBit)  > 0))
    {
        if (cpu->halt) {
            cpu->AddCycles(4);
        }

        cpu->halt = false;
    }
}

bool InterruptService::CheckInterrupts() {
    DisableHalt();
    if (cpu->GetIME() != 0x00) {
        uint8_t ie = mmu->Read(IE);
        uint8_t flags = mmu->Read(IFLAGS);

        // if bit0 (vBlankBit) is enable on IE and 0xFF0F
        if ((ie & flags & vBlankBit) > 0) {
            dispatch = vBlankBit;
            return true;
        }

        // if bit1 (lcdStatBit) is enable on IE and 0xFF0F
        if ((ie & flags & lcdStatBit) > 0) {
            dispatch = lcdStatBit;
            return true;
        }

        // if bit2 (timerBit) is enable on IE and 0xFF0F
        if ((ie & flags & timerBit) > 0) {
            dispatch = timerBit;
            return true;
        }

        // if bit3 (serialBit) is enable on IE and 0xFF0F
        if ((ie & flags & serialBit) > 0) {
            dispatch = serialBit;
            return true;
        }

        // if bit4 (joyPadBit) is enable on IE and 0xFF0F
        if ((ie & flags & joyPadBit) > 0) {
            dispatch = joyPadBit;
            return true;
        }
    }

    dispatch = 0xFF;
    return false;
}

void InterruptService::Dispatch() {
    if ((cpu->GetIME() != 0x00)) {
        switch (dispatch) {
            case vBlankBit: {
                VBlankInterrupt();
            }
                break;
            case lcdStatBit: {
                LCDStatInterrupt();
            }
                break;
            case timerBit: {
                TimerInterrupt();
            }
                break;
            case serialBit: {
                SerialInterrupt();
            }
                break;
            case joyPadBit: {
                JoyPadInterrupt();
            }
                break;
            default:
                break;
        }
    }
}

void InterruptService::VBlankInterrupt() {
    uint8_t flags = mmu->Read(IFLAGS);
    flags &= ~vBlankBit;
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(20);
    cpu->programCounter = vBlankISR;
    mmu->Write(IFLAGS, flags);
}

void InterruptService::LCDStatInterrupt() {
    uint8_t flags = mmu->Read(IFLAGS);
    flags &= ~lcdStatBit;
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(20);
    cpu->programCounter = lcdStatISR;
    mmu->Write(IFLAGS, flags);
}

void InterruptService::TimerInterrupt() {
    uint8_t flags = mmu->Read(IFLAGS);
    flags &= ~timerBit;
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(20);
    cpu->programCounter = timerISR;
    mmu->Write(IFLAGS, flags);
}

void InterruptService::SerialInterrupt() {
    uint8_t flags = mmu->Read(IFLAGS);
    flags &= ~serialBit;
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(20);
    cpu->programCounter = serialISR;
    mmu->Write(IFLAGS, flags);
}

void InterruptService::JoyPadInterrupt() {
    uint8_t flags = mmu->Read(IFLAGS);
    flags &= ~joyPadBit;
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(20);
    cpu->programCounter = joyPadISR;
    mmu->Write(IFLAGS, flags);
}
