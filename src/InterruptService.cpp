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
    if ((ie & flags & vBlankBit > 0)  || 
        (ie & flags & lcdStatBit > 0) ||
        (ie & flags & timerBit > 0)   ||
        (ie & flags & serialBit > 0)  ||
        (ie & flags & joyPadBit > 0))
    {
        cpu->halt = false;
    }
}

void InterruptService::CheckInterrupts() {
    DisableHalt();
    if (cpu->GetIME()) {
        uint8_t ie = mmu->Read(IE);
        uint8_t flags = mmu->Read(IFLAGS);
        
        // if bit0 (vBlankBit) is enable on IE and 0xFF0F
        if ((ie & flags & vBlankBit > 0)) {
            flags &= ~vBlankBit;
            VBlankInterrupt();
        }
        
        // if bit1 (lcdStatBit) is enable on IE and 0xFF0F
        if ((ie & flags & lcdStatBit > 0)) {
            flags &= ~lcdStatBit;
            LCDStatInterrupt();
        }
        
        // if bit2 (timerBit) is enable on IE and 0xFF0F
        if ((ie & flags & timerBit > 0)) {
            flags &= ~timerBit;
            TimerInterrupt();
        }
        
        // if bit3 (serialBit) is enable on IE and 0xFF0F
        if ((ie & flags & serialBit > 0)) {
            flags &= ~serialBit;
            SerialInterrupt();
        }
        
        // if bit4 (joyPadBit) is enable on IE and 0xFF0F
        if ((ie & flags & joyPadBit > 0)) {
            flags &= ~joyPadBit;
            JoyPadInterrupt();
        }
        
        mmu->Write(IFLAGS, flags);
    }
}

void InterruptService::PushFullRegisters() {
    uint16_t af = cpu->Get(FULL_REGISTERS::AF);
    uint16_t bc = cpu->Get(FULL_REGISTERS::BC);
    uint16_t de = cpu->Get(FULL_REGISTERS::DE);
    uint16_t hl = cpu->Get(FULL_REGISTERS::HL);
    
    cpu->PushSP(af);
    cpu->PushSP(bc);
    cpu->PushSP(de);
    cpu->PushSP(hl);
}

void InterruptService::PopFullRegisters() {
    uint16_t hl = cpu->PopSP16();
    uint16_t de = cpu->PopSP16();
    uint16_t bc = cpu->PopSP16();
    uint16_t af = cpu->PopSP16();
    
    cpu->Set(FULL_REGISTERS::AF, af);
    cpu->Set(FULL_REGISTERS::BC, bc);
    cpu->Set(FULL_REGISTERS::DE, de);
    cpu->Set(FULL_REGISTERS::HL, hl);
}

void InterruptService::VBlankInterrupt() {
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(12);
    cpu->programCounter = vBlankISR;
}

void InterruptService::LCDStatInterrupt() {
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(12);
    cpu->programCounter = lcdStatISR;
}

void InterruptService::TimerInterrupt() {
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(12);
    cpu->programCounter = timerISR;
}

void InterruptService::SerialInterrupt() {
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(12);
    cpu->programCounter = serialISR;
}

void InterruptService::JoyPadInterrupt() {
    cpu->SetIME(0x00);
    cpu->PushSP(cpu->programCounter);
    cpu->AddCycles(12);
    cpu->programCounter = joyPadISR;
}