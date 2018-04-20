// TimerService.cpp

#include "TimerService.h"

TimerService::TimerService(CPU* c, MMU* m, InterruptService* i) {
    cpu = c;
    mmu = m;
    is = i;
}

TimerService::~TimerService() {
    delete is;
    delete cpu;
    delete mmu;
}

void TimerService::Increment() {
    uint16_t cycles = cpu->GetCycles();
    // divider clock
    divClock += cycles;
    CheckDiv();

    // counter mess
    timaClock += cycles;
    CheckTima();
}

void TimerService::CheckTima() {
    uint8_t control = mmu->Read(controlAddr);
    uint32_t clockSpeed;

    if (control & runningBit) {
         // divide the operation speed of the cpu clock by the speed set in the control to get the "clockSpeed" variable value
        switch (control & speedBits) {
            case 0: // 4096hz
                clockSpeed = 1024;
                break;
            case 1: // 262144hz
                clockSpeed = 16;
                break;
            case 2: // 65536hz
                clockSpeed = 64;
                break;
            case 3: // 16384hz
                clockSpeed = 256;
                break;
        }


        if (timaClock >= clockSpeed) {
            StepTima();
        }
    }
}

void TimerService::CheckDiv() {
    if (divClock >= 0xFF) {
        uint8_t div = mmu->Read(divAddr);
        div++;
        mmu->WriteTimers(divAddr, div);
        divClock %= 256; // we want to keep ALL OF THE CLOCKS
    }
}

void TimerService::StepTima() {
    uint16_t tima = mmu->Read(timaAddr);
    tima++;
    printf("%d\n", tima);
    // timaClock = 0;
    timaClock %= 256; // we want to keep ALL OF THE CLOCKS

    if (tima >= 256) {
        tima = mmu->Read(moduloAddr);
        // cpu->AddCycles(4);
        TimaInterrupt();
    }

    mmu->Write(timaAddr, (uint8_t)(tima & 0xFF));
}

void TimerService::TimaInterrupt() {
    uint8_t flags = mmu->Read(InterruptService::IFLAGS);
    flags |= InterruptService::timerBit;
    mmu->Write(InterruptService::IFLAGS, flags);
    // cpu->AddCycles(4);
}
