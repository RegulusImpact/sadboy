#ifndef TimerService_H
#define TimerService_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

#include <iostream>

#include "Utils.h"

#include "CPU.h"
#include "MMU.h"
#include "InterruptService.h"

class TimerService {
private:
    // memory registers
    const uint16_t divAddr = 0xFF04; // div
    const uint16_t timaAddr = 0xFF05; // tima
    const uint16_t moduloAddr  = 0xFF06; // tma

    // bit 0-1 speed
    //      00: 4096hz
    //      01: 262144hz
    //      10: 65536hz
    //      11: 16384hz
    // bit 2: running
    //      1: run
    //      0: stop
    // bit 3-7: unused
    const uint16_t controlAddr = 0xFF07;
    const uint8_t speedBits = (Utils::BIT_0 + Utils::BIT_1);
    const uint8_t runningBit = Utils::BIT_2;

    // internal variables to keep track of progress
    uint32_t divClock; // horse shit
    uint32_t timaClock; // this determines when trigger a tima step; i.e. clock > clock_speed

    CPU* cpu;
    MMU* mmu;
    InterruptService* is;

public:
    TimerService(CPU* c, MMU* m, InterruptService* i);
    ~TimerService();

    void Increment(); // increment our clock
    void CheckTima();
    void CheckDiv();
    void StepTima();

    void TimaInterrupt();

};

#endif
