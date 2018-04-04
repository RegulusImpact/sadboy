// InterruptService.h
#ifndef InterruptService_H
#define InterruptService_H

#include "MMU.h"
#include "CPU.h"

class InterruptService {
private:
    MMU* mmu;
    CPU* cpu;
    
    const uint16_t  IE = 0xFFFF;
    const uint16_t  IFLAGS = 0xFF0F;
    
    const uint8_t  vBlankBit = 0b00000001; // bit 0
    const uint8_t lcdStatBit = 0b00000010; // bit 1
    const uint8_t   timerBit = 0b00000100; // bit 2
    const uint8_t  serialBit = 0b00001000; // bit 3
    const uint8_t  joyPadBit = 0b00010000; // bit 4
    
    const uint16_t  vBlankISR = 0x0040;
    const uint16_t lcdStatISR = 0x0048;
    const uint16_t   timerISR = 0x0050;
    const uint16_t  serialISR = 0x0058;
    const uint16_t  joyPadISR = 0x0060;
    
    void PushFullRegisters();
    void PopFullRegisters();
    
public:
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