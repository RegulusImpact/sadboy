// CPU.h
#ifndef CPU_H
#define CPU_H

#include <iostream>
// uintX_t
#include <cstdint>
// size_t
#include <cstddef>
#include <string.h>

#include "MMU.h"
#include "Graphics/TextGPU.h"

// testing
#include "json/json.h"
#include <fstream>

/* Full and Half Carry */
/* Source: https://github.com/nnarain/gameboycore/blob/54fbba36beab61e0e6f4d37a090aa2b84b81a748/src/core/bitutil.h */
#define IS_HALF_CARRY(x, y) ((( ((x) & 0x0F) + ((y) & 0x0F) ) & (0x10)) != 0)
#define IS_FULL_CARRY(x, y) ((( ( (x)&0x0FF ) + ( ((y)&0x0FF) )  ) & 0x100 ) != 0)

#define IS_HALF_CARRY16(x, y) ((( ((x) & 0x0FFF) + ((y) & 0x0FFF) ) & (0x1000)) != 0)
#define IS_FULL_CARRY16(x, y) ((( ( (x)&0x0FFFF ) + ( ((y)&0x0FFFF) )  ) & 0x10000 ) != 0)

#define IS_HALF_BORROW(x, y) (  ((x)&0x0F) < ((y)&0x0F)  )
#define IS_FULL_BORROW(x, y) (  ((x)&0xFF) < ((y)&0xFF)  )

/* Two's Complement uint8_t to int8_t */
/* stolen from pablo's brain
    if (value > 127) {
        value = -((~value + 1) & 255); 
     }
*/
#define TWOS_COMPLEMENT(x) ( static_cast<int8_t>(x) )

#ifndef REGISTER_ENUM
#define REGISTER_ENUM
    enum REGISTERS {
        B,  // 0
        C,  // 1
        D,  // 2
        E,  // 3
        H,  // 4
        L,  // 5
        F,  // 6
        A  // 7
    };
#endif


#ifndef FULL_REGISTER_ENUM
#define FULL_REGISTER_ENUM
    enum FULL_REGISTERS {
        BC = 0,
        DE = 1,
        HL = 2,
        AF = 3
    };
#endif

class CPU {
private:
    std::uint8_t regs[8];
    std::uint8_t cycles;
    
    std::uint8_t IME;
    
    MMU* mmu;
    TextGPU* gpu;
    
    void CheckOpcode(uint8_t opcode);
    void CheckExtension(uint8_t opcode);
    std::uint8_t ReadPC();
    std::uint16_t ReadPC16();
    
    REGISTERS RegisterParse(uint8_t opcode);
    FULL_REGISTERS FullRegisterParse(uint8_t opcode);
    
public:
    CPU();
    CPU(MMU* m);
    ~CPU();
    
    std::uint16_t programCounter;
    std::uint16_t stackPointer;
    std::uint8_t opcode;
    bool halt;

#ifndef FLAGS_ENUM
#define FLAGS_ENUM
    enum FLAGS {
        Z  = 0b10000000,
        N  = 0b01000000,
        H  = 0b00100000,
        CY = 0b00010000
    };
#endif

    std::uint16_t Get(REGISTERS);
    std::uint16_t Get(FULL_REGISTERS);
    
    void Set(REGISTERS, uint8_t);
    void Set(FULL_REGISTERS r, std::uint16_t);
    
    void Set(FLAGS f);
    void Clear(FLAGS f);
    
    void SetZ(bool b);
    void SetN(bool b);
    void SetCY(bool b);
    void SetH(bool b);
    
    bool GetZ();
    bool GetN();
    bool GetCY();
    bool GetH();
    
    uint8_t GetIME();
    void SetIME(uint8_t ime);
    
    uint8_t GetCycles();
    void SetCycles(uint8_t c);
    void AddCycles(uint8_t c);
    
    void Increment(REGISTERS);
    void Decrement(REGISTERS);
    void Increment(FULL_REGISTERS);
    void Decrement(FULL_REGISTERS);
    
    uint8_t PopSP();
    uint16_t PopSP16();
    
    void PushSP(uint8_t value);
    void PushSP(uint16_t value);
    
    void Read();
    
    void CheckRegisters();
};

#endif