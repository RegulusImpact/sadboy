// InterruptService.h
#ifndef InterruptService_H
#define InterruptService_H

#include "MMU.h"
#include "CPU.h"
#include "Utils.h"

class InterruptService {
private:
    MMU* mmu;
    CPU* cpu;

    void PushFullRegisters();
    void PopFullRegisters();

public:
    static const uint16_t  IE = 0xFFFF;
    static const uint16_t  IFLAGS = 0xFF0F;

    static const uint8_t  vBlankBit = Utils::BIT_0; // bit 0
    static const uint8_t lcdStatBit = Utils::BIT_1; // bit 1
    static const uint8_t   timerBit = Utils::BIT_2; // bit 2
    static const uint8_t  serialBit = Utils::BIT_3; // bit 3
    static const uint8_t  joyPadBit = Utils::BIT_4; // bit 4

    static const uint16_t  vBlankISR = 0x0040;
    static const uint16_t lcdStatISR = 0x0048;
    static const uint16_t   timerISR = 0x0050;
    static const uint16_t  serialISR = 0x0058;
    static const uint16_t  joyPadISR = 0x0060;

    InterruptService(CPU* c, MMU* m);
    ~InterruptService();

    void DisableHalt();
    void CheckInterrupts();

    void VBlankInterrupt();
    void LCDStatInterrupt();
    void TimerInterrupt();
    void SerialInterrupt();
    void JoyPadInterrupt();
};

#endif
