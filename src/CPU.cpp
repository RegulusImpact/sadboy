// CPU.cpp
#include "CPU.h"

CPU::CPU(void): CPU(new MMU(new Cartridge("cart/bgbtest.gb"))) { }

CPU::CPU(MMU* m) {
    mmu = m;

    IME = 0xFF;
    halt = false;

    for (int i = 0; i < 8; i++) {
        regs[i] = 0;
    }

    programCounter = 0;
    stackPointer = 0;
    cycles = 0;
}

CPU::~CPU(void) {
    // https://stackoverflow.com/questions/24292194/g-gives-the-warning-message-deleting-array-in-virtual-destructor-what-does-th
    // delete[] regs;
    delete mmu;
}

std::uint16_t CPU::Get(REGISTERS r) {
    return regs[r];
}

std::uint16_t CPU::Get(FULL_REGISTERS r) {
    std::uint16_t value = 0;
    switch (r) {
        case FULL_REGISTERS::AF:
            value = regs[REGISTERS::A] << 8 | regs[REGISTERS::F];
            break;
        case FULL_REGISTERS::BC:
            value = regs[REGISTERS::B] << 8 | regs[REGISTERS::C];
            break;
        case FULL_REGISTERS::DE:
            value = regs[REGISTERS::D] << 8 | regs[REGISTERS::E];
            break;
        case FULL_REGISTERS::HL:
            value = regs[REGISTERS::H] << 8 | regs[REGISTERS::L];
            break;
    }

    return value;
}

void CPU::Set(REGISTERS r, std::uint8_t value) {
    regs[r] = value;
}

void CPU::Set(FULL_REGISTERS r, std::uint16_t value) {
    uint8_t hi = (uint8_t)(value >> 8);
    uint8_t lo = (uint8_t)(value);
    switch (r) {
        case FULL_REGISTERS::AF:
            regs[REGISTERS::A] = hi;
            regs[REGISTERS::F] = lo;
            break;
        case FULL_REGISTERS::BC:
            regs[REGISTERS::B] = hi;
            regs[REGISTERS::C] = lo;
            break;
        case FULL_REGISTERS::DE:
            regs[REGISTERS::D] = hi;
            regs[REGISTERS::E] = lo;
            break;
        case FULL_REGISTERS::HL:
            regs[REGISTERS::H] = hi;
            regs[REGISTERS::L] = lo;
            break;
    }
}

void CPU::Set(FLAGS f) {
    uint8_t flag = regs[REGISTERS::F];
    flag = flag | f;
    Set(REGISTERS::F, flag);
}

void CPU::Clear(FLAGS f) {
    uint8_t flag = regs[REGISTERS::F];
    flag = flag & ~f;
    Set(REGISTERS::F, flag);
}

void CPU::SetZ(bool b) {
    if (b)
        Set(FLAGS::Z);
    else
        Clear(FLAGS::Z);
}

void CPU::SetN(bool b) {
    if (b)
        Set(FLAGS::N);
    else
        Clear(FLAGS::N);
}

void CPU::SetCY(bool b) {
    if (b)
        Set(FLAGS::CY);
    else
        Clear(FLAGS::CY);
}

void CPU::SetH(bool b) {
    if (b)
        Set(FLAGS::H);
    else
        Clear(FLAGS::H);
}

bool CPU::GetZ() {
    // FLAGS::Z == 0b10000000
    return regs[REGISTERS::F] & FLAGS::Z;
}

bool CPU::GetN() {
    // FLAGS::N == 0b01000000
    return regs[REGISTERS::F] & FLAGS::N;
}

bool CPU::GetCY() {
    // FLAGS::CY == 0b00100000
    return regs[REGISTERS::F] & FLAGS::CY;
}

bool CPU::GetH() {
    // FLAGS::H == 0b00010000
    return regs[REGISTERS::F] & FLAGS::H;
}

uint8_t CPU::GetIME() {
    return IME;
}

void CPU::SetIME(uint8_t ime) {
    IME = ime;
}

uint8_t CPU::GetCycles() {
    return cycles;
}

void CPU::SetCycles(uint8_t c) {
    cycles = c;
}

void CPU::AddCycles(uint8_t c) {
    cycles += c;
}

void CPU::Increment(REGISTERS r) {
    regs[r]++;
}

void CPU::Decrement(REGISTERS r) {
    regs[r]--;
}

void CPU::Increment(FULL_REGISTERS r) {
    std::uint16_t value = 0;
    REGISTERS hi;
    REGISTERS lo;
    switch (r) {
        case FULL_REGISTERS::AF:
            hi = REGISTERS::A;
            lo = REGISTERS::F;
            break;
        case FULL_REGISTERS::BC:
            hi = REGISTERS::B;
            lo = REGISTERS::C;
            break;
        case FULL_REGISTERS::DE:
            hi = REGISTERS::D;
            lo = REGISTERS::E;
            break;
        case FULL_REGISTERS::HL:
            hi = REGISTERS::H;
            lo = REGISTERS::L;
            break;
    }

    value = (regs[hi] << 8) | regs[lo];
    value++;
    uint8_t top = (value & 0xFF00) >> 8;
    uint8_t bot = (value & 0x00FF);
    Set(hi, top);
    Set(lo, bot);
}

void CPU::Decrement(FULL_REGISTERS r) {
    std::uint16_t value = 0;
    REGISTERS hi;
    REGISTERS lo;
    switch (r) {
        case FULL_REGISTERS::AF:
            hi = REGISTERS::A;
            lo = REGISTERS::F;
            break;
        case FULL_REGISTERS::BC:
            hi = REGISTERS::B;
            lo = REGISTERS::C;
            break;
        case FULL_REGISTERS::DE:
            hi = REGISTERS::D;
            lo = REGISTERS::E;
            break;
        case FULL_REGISTERS::HL:
            hi = REGISTERS::H;
            lo = REGISTERS::L;
            break;
    }

    value = (regs[hi] << 8) | regs[lo];
    value--;
    uint8_t top = (value & 0xFF00) >> 8;
    uint8_t bot = (value & 0x00FF);
    Set(hi, top);
    Set(lo, bot);
}

std::uint8_t CPU::ReadPC() {
    uint8_t n = mmu->Read(programCounter++);

    //fprintf(stderr,("Input: 0x%.2X\n", n);

    return n;
}

std::uint16_t CPU::ReadPC16() {
    std::uint8_t lo = ReadPC();
    std::uint8_t hi = ReadPC();
    std::uint16_t addr = (((uint16_t)hi) << 8) | lo;

    return addr;
}

uint8_t CPU::PopSP() {
    uint16_t addr = stackPointer++;
    uint8_t n = mmu->Read(addr);
    //mmu->Write(addr, (uint8_t)0x00);
    return n;
}

uint16_t CPU::PopSP16() {
    uint16_t addr = stackPointer++;
    stackPointer++;
    uint16_t n = mmu->Read16Bit(addr);
    //mmu->Write(addr, (uint16_t)0x00);
    return n;
}

void CPU::PushSP(uint8_t value) {
    uint16_t addr = --stackPointer;
    mmu->Write(addr, value);
}

void CPU::PushSP(uint16_t value) {
    uint8_t hi = (uint8_t)((value >> 8) & 0xFF);
    uint8_t lo = (uint8_t)((value) & 0xFF);
    PushSP(hi);
    PushSP(lo);
}

int loopCounter = 0;
void CPU::Read() {
    if (!halt) {
        // uint16_t pc = programCounter;
        uint8_t opcode = ReadPC();

        // if (pc == 0xC04F) {
        //     loopCounter++;
        // }

        // if (mmu->enableDebugger && loopCounter > 5) {
        //     std::fprintf(stderr, "%.4X: %.2X\n", pc, opcode);
        // }
        CheckOpcode(opcode);
    }
}

REGISTERS CPU::RegisterParse(uint8_t opcode) {
    return (REGISTERS)(uint8_t)((opcode >> 3) & 0b0111); // 8 max registers
}

FULL_REGISTERS CPU::FullRegisterParse(uint8_t opcode) {
    uint8_t shft = opcode >> 4;
    uint8_t and3 = shft & 3;
    FULL_REGISTERS r = (FULL_REGISTERS)and3;

    return r; // 4 max full registers
}


void CPU::CheckOpcode(std::uint8_t opcode) {
    cycles = 0;
    switch(opcode) {
        // LD nn, n
        case 0x06: // LD B, n
        case 0x0E: // LD C, n
        case 0x16: // LD D, n
        case 0x1E: // LD E, n
        case 0x26: // LD H, n
        case 0x2E: // LD L, n
        {
            REGISTERS r = RegisterParse(opcode);
            std::uint8_t n = ReadPC();
            Set(r, n);
            cycles = 8;
        }
            break;
        // ld reg,reg
        // some convenient (and literal) black magic
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // ld b,reg
        case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4f: // ld c,reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // ld d,reg
        case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5f: // ld e,reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // ld h,reg
        case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6f: // ld l,reg
        case 0x78: case 0x79: case 0x7a: case 0x7b: case 0x7c: case 0x7d: case 0x7f: // ld a,reg
        {
            REGISTERS dst = RegisterParse(opcode);
            REGISTERS src = (REGISTERS)(opcode & 0b00000111);
            Set(dst, Get(src));
            cycles = 4;
        }
            break;
        // LD reg, (HL)
        case 0x46: // ld b, (hl)
        case 0x4E: // ld c, (hl)
        case 0x56: // ld d, (hl)
        case 0x5E: // ld e, (hl)
        case 0x66: // ld h, (hl)
        case 0x6E: // ld l, (hl)
        case 0x7E: // ld a, (hl)
        {
            std::uint16_t mem = Get(FULL_REGISTERS::HL);
            uint8_t n = mmu->Read(mem);
            Set(RegisterParse(opcode), n);
            cycles = 8;
        }
            break;
        // LD (HL), reg
        case 0x70: // ld (hl), b
        case 0x71: // ld (hl), c
        case 0x72: // ld (hl), d
        case 0x73: // ld (hl), e
        case 0x74: // ld (hl), h
        case 0x75: // ld (hl), l
        case 0x77: // ld (hl), a
        {
            std::uint16_t mem = Get(FULL_REGISTERS::HL);
            REGISTERS r = (REGISTERS)(opcode & 7);
            uint8_t val = Get(r);
            mmu->Write(mem, val);

            cycles = 8;
        }
            break;
        case 0x36: // ld (hl), n
        {
            std::uint8_t n = ReadPC();
            std::uint16_t mem = Get(FULL_REGISTERS::HL);
            mmu->Write(mem, n);
            cycles = 12;
        }
            break;
        // LD A, reg
        case 0x0A: // ld a, (bc)
        {
            std::uint16_t mem = Get(FULL_REGISTERS::BC);
            std::uint8_t n = mmu->Read(mem);
            Set(REGISTERS::A, n);
            cycles = 8;
        }
            break;
        case 0x1A: // ld a, (de)
        {
            std::uint16_t addr = Get(FULL_REGISTERS::DE);
            std::uint8_t n = mmu->Read(addr);

            Set(REGISTERS::A, n);
            cycles = 8;
        }
            break;
        case 0xFA: // ld a, (nn)
        {
            std::uint16_t addr = ReadPC16();
            std::uint8_t n = mmu->Read(addr);

            Set(REGISTERS::A, n);
            cycles = 16;
        }
            break;
        case 0x3E: // ld a, #
        {
            std::uint8_t n = ReadPC();
            Set(REGISTERS::A, n);
            cycles = 16;
        }
            break;
        case 0x02: // ld (bc),a
        {
            std::uint16_t mem = Get(FULL_REGISTERS::BC);
            mmu->Write(mem, regs[REGISTERS::A]);
            cycles = 8;
        }
            break;
        case 0x12: // ld (de),a
        {
            std::uint16_t mem = Get(FULL_REGISTERS::DE);
            mmu->Write(mem, regs[REGISTERS::A]);
            cycles = 8;
        }
            break;
        case 0xEA: // ld (nn),a
        {
            std::uint16_t addr = ReadPC16();
            mmu->Write(addr, regs[REGISTERS::A]);
            cycles = 16;
        }
            break;
        case 0xF2: // ld a, (c)
        {
            uint16_t addr = 0xFF00 | regs[REGISTERS::C];
            std::uint8_t n = mmu->Read(addr);
            Set(REGISTERS::A, n);
            cycles = 8;
        }
            break;
        case 0xE2: // ld (c), a
        {
            uint16_t addr = 0xFF00 | regs[REGISTERS::C];
            uint8_t n = regs[REGISTERS::A];

            mmu->Write(addr, n);
            cycles = 8;
        }
            break;
        case 0x3A: // ld a,(hld) ; ld a,(hl-) ; ldd a,(hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            Decrement(FULL_REGISTERS::HL);
            uint8_t n = mmu->Read(addr);
            Set(REGISTERS::A, n);
            cycles = 8;
        }
            break;
        case 0x32: // ld (hld),a ; ld (hl-),a ; ldd (hl),a
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            Decrement(FULL_REGISTERS::HL);
            uint8_t n = regs[REGISTERS::A];
            mmu->Write(addr, n);
            cycles = 8;
        }
            break;
        case 0x2A: // ld a,(hli) ; ld a,(hl+) ; ldi a,(hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            Increment(FULL_REGISTERS::HL);
            uint8_t n = mmu->Read(addr);
            Set(REGISTERS::A, n);
            cycles = 8;
        }
            break;
        case 0x22: // ld (hli),a ; ld (hl+),a ; ldi (hl),a
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            Increment(FULL_REGISTERS::HL);
            uint8_t n = regs[REGISTERS::A];
            mmu->Write(addr, n);
            cycles = 8;
        }
            break;
        case 0xE0: // ldh (n), A
        {
            uint8_t n = ReadPC();
            uint16_t addr = 0xFF00 | n;
            uint8_t a = regs[REGISTERS::A];
            mmu->Write(addr, a);
            cycles = 12;
        }
            break;
        case 0xF0: // ldh A,(n)
        {
            uint8_t n = ReadPC();
            uint16_t addr = 0xFF00 | n;
            uint8_t a = mmu->Read(addr);
            Set(REGISTERS::A, a);
            cycles = 12;
        }
            break;
        case 0x01: //ld bc, nn
        case 0x11: //ld de, nn
        case 0x21: //ld hl, nn
        {
            uint16_t nn = ReadPC16();
            // 0000 0001
            // 0001 0001
            // 0010 0001
            FULL_REGISTERS r = FullRegisterParse(opcode);
            Set(r, nn);

            cycles = 12;
        }
            break;
        case 0x31: //ld sp, nn
        {
            uint16_t nn = ReadPC16();
            stackPointer = nn;
            cycles = 12;
        }
            break;
        case 0xF9: // ld sp, hl
        {

            uint16_t hl = Get(FULL_REGISTERS::HL);
            stackPointer = hl;
            cycles = 8;
        }
            break;
        case 0xF8: // ld hl,sp+n ; ldhl sp,n
        {
            int8_t n = TWOS_COMPLEMENT(ReadPC());
            uint16_t spn = stackPointer + n;
            Set(FULL_REGISTERS::HL, spn);

            SetZ(false);
            SetN(false);
            SetCY(IS_FULL_CARRY(stackPointer, n));
            SetH(IS_HALF_CARRY(stackPointer, n));

            cycles = 12;
        }
            break;
        case 0x08: // ld (nn), SP
        {
            // this is off by one based on the write hi; dec; write lo of the mmu
            // we want sp_lo on nn, and sp_hi on nn+1
            uint16_t nn = ReadPC16()+1;
            mmu->Write(nn, stackPointer);

            cycles = 20;
        }
            break;
        case 0xC5: // push bc
        case 0xD5: // push de
        case 0xE5: // push hl
        case 0xF5: // push af
        {
            // stackPointer--;
            // uint16_t addr = stackPointer--;
            uint16_t reg = Get(FullRegisterParse(opcode));
            // mmu->Write(addr, reg);
            PushSP(reg);
            cycles = 16;
        }
            break;
        case 0xC1: // pop bc
        case 0xD1: // pop de
        case 0xE1: // pop hl
        case 0xF1: // pop af
        {
            //std::cout << "POSSIBLE OFF BY ONE POINT." << std::endl;
            // uint16_t addr = stackPointer++; // write16 increments the address
            // stackPointer++;
            uint16_t nn = PopSP16();// mmu->Read16Bit(addr);
            // remove values from stack
            // mmu->Write(addr, (uint16_t)0x0000);
            FULL_REGISTERS r = FullRegisterParse(opcode);
            if (opcode == 0xF1) {
                //  0xFF = A register
                //  0xF0 = F register
                nn &= 0xFFF0;
            }
            Set(r, nn);

            cycles = 16;
        }
            break;

        /* 8 BIT ALU */

        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // add a,reg
        {

            REGISTERS r = (REGISTERS)(opcode & 7);
            uint8_t val = Get(r);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a + val;
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(false);
            SetCY(IS_FULL_CARRY(a, val));
            SetH(IS_HALF_CARRY(a, val));

            cycles = 4;
        }
            break;
        case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8f: // adc a,reg
        {
            bool carry = GetCY();

            REGISTERS r = (REGISTERS)(opcode & 7);
            uint8_t val = Get(r);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a + val + (carry ? 1 : 0);
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(false);
            SetCY(((a ^ val ^ tot) & 0x100) != 0x00);
            SetH(((a ^ val ^ tot) & 0x10) != 0x00);

            cycles = 4;
        }
            break;
        case 0x86: // add a, (hl)
        case 0x8E: // adc a, (hl)
        {
            bool carry = GetCY() && ((opcode & 8) != 0);
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t val = mmu->Read(addr) + (carry ? 1 : 0);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a + val;
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(false);
            SetCY(IS_FULL_CARRY(a, val));
            SetH(IS_HALF_CARRY(a, val));

            cycles = 8;
        }
            break;
        case 0xC6: // add a, (nn)
        case 0xCE: // adc a, (nn)
        {
            bool carry = GetCY() && ((opcode & 8) != 0);
            uint8_t val = ReadPC() + (carry ? 1 : 0);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a + val;
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(false);
            SetCY(IS_FULL_CARRY(a, val));
            SetH(IS_HALF_CARRY(a, val));

            cycles = 8;
        }
            break;
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // sub a, reg
        case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F: // sbc a, reg
        {
            bool carry = GetCY() && ((opcode & 8) != 0);

            REGISTERS r = (REGISTERS)(opcode & 7);
            uint8_t val = Get(r) + (carry ? 1 : 0);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a - val;
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(true);
            SetCY(IS_FULL_BORROW(a, val));
            SetH(IS_HALF_BORROW(a, val));

            cycles = 4;
        }
            break;
        case 0x96: // sub a, (hl)
        case 0x9E: // sbc a, (hl)
        {
            bool carry = GetCY() && ((opcode & 8) != 0);

            uint8_t val = Get(FULL_REGISTERS::HL) + (carry ? 1 : 0);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a - val;
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(true);
            SetCY(IS_FULL_BORROW(a, val));
            SetH(IS_HALF_BORROW(a, val));
            cycles = 8;
        }
            break;
        case 0xD6: // sub a, (nn)
        case 0xDE: // sbc a, (nn) ; documentation labels the opcode as ???
        {
            bool carry = GetCY() && ((opcode & 8) != 0);

            uint8_t val = ReadPC() + (carry ? 1 : 0);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a - val;
            Set(REGISTERS::A, tot);

            SetZ(0 == tot);
            SetN(true);
            SetCY(IS_FULL_BORROW(a, val));
            SetH(IS_HALF_BORROW(a, val));

            cycles = 8;
        }
            break;
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: // and a, reg
        {
            uint8_t val = Get((REGISTERS)(opcode & 0b00000111));
            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a & val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetH(true);
            SetCY(false);

            cycles = 4;
        }
            break;
        case 0xA6: // and a, (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t val = mmu->Read(addr);

            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a & val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetH(true);
            SetCY(false);

            cycles = 8;
        }
            break;
        case 0xE6: // and a, (nn)
        {
            uint8_t val = ReadPC();

            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a & val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetH(true);
            SetCY(false);

            cycles = 8;
        }
            break;
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: // or a, reg
        {
            uint8_t val = Get((REGISTERS)(opcode & 0b00000111));
            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a | val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetH(false);
            SetCY(false);

            cycles = 4;
        }
            break;
        case 0xB6: // or a, (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t val = mmu->Read(addr);

            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a | val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetH(false);
            SetCY(false);

            cycles = 8;
        }
            break;
        case 0xF6: // or a, (nn)
        {
            uint8_t val = ReadPC();

            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a | val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetH(false);
            SetCY(false);

            cycles = 8;
        }
            break;
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: // xor a, reg
        {
            uint8_t val = Get((REGISTERS)(opcode & 0b00000111));
            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a ^ val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetCY(false);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0xAE: // xor a, (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t val = mmu->Read(addr);
            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a ^ val;
            Set(REGISTERS::A, result);

            SetZ(0 == result);
            SetN(false);
            SetCY(false);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0xEE: // xor a, (nn)
        {
            uint8_t val = ReadPC();
            uint8_t a = Get(REGISTERS::A);
            uint8_t result = a ^ val;
            Set(REGISTERS::A, result);
            // fprintf(stderr, "PC: 0x%.4X    A: 0x%.2X    OPERAND: %d d    RESULT: 0x%.2X    F: 0x%.2X\n",programCounter,a,val,result,Get(REGISTERS::F));

            SetZ(0 == result);
            SetN(false);
            SetCY(false);
            SetH(false);


            cycles = 8;
        }
            break;

        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: // cp a, reg
        {
            uint8_t val = Get((REGISTERS)(opcode & 0b00000111));
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a - val;

            SetZ(0 == tot);
            SetN(true);
            SetCY(IS_FULL_BORROW(a, val));
            SetH(IS_HALF_BORROW(a, val));

            cycles = 4;
        }
            break;
        case 0xBE: // cp a, (hl)
        {
            uint16_t hl = Get(FULL_REGISTERS::HL);
            uint8_t val = mmu->Read(hl);
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a - val;
            SetZ(0 == tot);
            SetN(true);
            SetCY(IS_FULL_BORROW(a, val));
            SetH(IS_HALF_BORROW(a, val));
            cycles = 8;
        }
            break;
        case 0xFE: // cp a, #
        {
            uint8_t val = ReadPC();
            uint8_t a = Get(REGISTERS::A);
            uint8_t tot = a - val;

            SetZ(0 == tot);
            SetN(true);
            SetCY(IS_FULL_BORROW(a, val));
            SetH(IS_HALF_BORROW(a, val));

            cycles = 8;
        }
            break;
        case 0x04: //increment b
        case 0x0C: //increment c
        case 0x14: //increment d
        case 0x1C: //increment e
        case 0x24: //increment h
        case 0x2C: //increment l
        //case 0x34: //increment f
        case 0x3C: //increment a
        {
            REGISTERS r = RegisterParse(opcode);
            uint8_t prev = Get(r);
            Increment(r);
            uint8_t next = Get(r);

            SetZ(0 == next);
            SetN(false);
            // unaffected SetCY(); //unaffected
            SetH(IS_HALF_CARRY(prev, 1));

            cycles = 4;
        }
            break;
        case 0x34: // incremenet (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t prev = mmu->Read(addr);
            uint8_t next = prev + 1;
            mmu->Write(addr, next);

            SetZ(0 == next);
            SetN(false);
            // unaffected SetCY(); //unaffected
            SetH(IS_HALF_CARRY(prev, 1));

            cycles = 12;
        }
            break;
        case 0x05: //dec b
        case 0x0D: //dec c
        case 0x15: //dec d
        case 0x1D: //dec e
        case 0x25: //dec h
        case 0x2D: //dec l
        //case 0x35: //dec f
        case 0x3D: //dec a
        {
            REGISTERS r = RegisterParse(opcode);
            uint8_t prev = Get(r);
            Decrement(r);
            uint8_t next = Get(r);

            SetZ(0 == next);
            SetN(true);
            // unaffected SetCY(); //unaffected
            SetH(IS_HALF_BORROW(prev, 1));

            cycles = 4;
        }
            break;
        case 0x35: // dec (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t prev = mmu->Read(addr);
            uint8_t next = prev - 1;
            mmu->Write(addr, next);

            SetZ(0 == next);
            SetN(true);
            // unaffected SetCY(); //unaffected
            SetH(IS_HALF_BORROW(prev, 1));

            cycles = 12;
        }
            break;
        case 0x09: case 0x19: case 0x29: // add hl, reg
        {
            uint16_t src = Get(FullRegisterParse(opcode));
            uint16_t hl = Get(FULL_REGISTERS::HL);
            uint16_t total = src + hl;
            Set(FULL_REGISTERS::HL, total);

            SetN(false);
            SetCY(IS_FULL_CARRY16(hl, src));
            SetH(IS_HALF_CARRY16(hl, src));

            cycles = 8;
        }
            break;
        case 0x39: // add hl, sp
        {
            uint16_t src = stackPointer;
            uint16_t hl = Get(FULL_REGISTERS::HL);
            uint16_t total = src + hl;
            Set(FULL_REGISTERS::HL, total);

            SetN(false);
            SetCY(IS_FULL_CARRY16(hl, src));
            SetH(IS_HALF_CARRY16(hl, src));

            cycles = 8;
        }
            break;
        case 0xE8: // add sp, n
        {
            int8_t n = TWOS_COMPLEMENT(ReadPC());
            uint16_t sp = stackPointer;
            stackPointer = sp + n;

            SetZ(false);
            SetN(false);
            SetCY(IS_FULL_CARRY16(sp, n));
            SetH(IS_HALF_CARRY16(sp, n));

            cycles = 16;
        }
            break;
        case 0x03: case 0x13: case 0x23: // inc nn
        {
            Increment(FullRegisterParse(opcode));

            cycles = 8;
        }
            break;
        case 0x33: // inc nn
        {
            stackPointer++;

            cycles = 8;
        }
            break;
        case 0x0B: case 0x1B: case 0x2B: // dec nn
        {
            Decrement(FullRegisterParse(opcode));

            cycles = 8;
        }
            break;
        case 0x3B: // dec nn
        {
            stackPointer--;

            cycles = 8;
        }
            break;
        case 0xCB: // extensions
        {
            CheckExtension(ReadPC());
        }
            break;
        case 0x27: // DAA
        {
            uint8_t a = Get(REGISTERS::A);
            uint8_t f = Get(REGISTERS::F);

            uint16_t op = a;

            if (! (f & FLAGS::N))
            {
                if ((f & FLAGS::H) || (op & 0xF) > 9)
                    op += 0x06;

                if ((f & FLAGS::CY) || op > 0x9F)
                    op += 0x60;
            }
            else
            {
                if (f & FLAGS::H)
                    op = (op - 6) & 0xFF;

                if (f & FLAGS::CY)
                    op -= 0x60;
            }

            f &= ~(FLAGS::H | FLAGS::Z);

            if ((op & 0x100) == 0x100)
                f |= FLAGS::CY;

            op &= 0xFF;

            if (op == 0)
                f |= FLAGS::Z;

            a = (uint8_t)op;

            Set(REGISTERS::A, a);
            Set(REGISTERS::F, f);

            cycles = 4;

        }
            break;
        case 0x2F: // cpl A
        {
            uint8_t a = ~Get(REGISTERS::A);
            Set(REGISTERS::A, a);

            SetN(true);
            SetH(true);

            cycles = 4;
        }
            break;
        case 0x3F: // ccf
        {
            SetCY(!GetCY());

            SetN(false);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0x37: // scf
        {
            SetCY(true);
            SetN(false);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0x00: // NOP
        {
            // std::cout << "NOP" << std::endl;
            cycles = 4;
        }
            break;
        case 0x76: // HALT
        {
            // std::cout << "HALT" << std::endl;
            cycles = 4;
        }
            break;
        case 0x10: // STOP
        {
            // std::cout << "STOP" << std::endl;
            cycles = 4;
        }
            break;
        case 0xF3: // di
        {
            // disable interrupts
            IME = 0x00;
            mmu->Write(0xFFFF, (uint8_t)0x00);
            cycles = 4;
        }
            break;
        case 0xFB: // ei
        {
            // enable interrupts
            IME = 0xFF;
            mmu->Write(0xFFFF, (uint8_t)0xFF);
            cycles = 4;
        }
            break;
        case 0x07: // rlca
        {
            uint8_t a = Get(REGISTERS::A);
            uint8_t c = a >> 7;
            a = (a << 1) | c;
            Set(REGISTERS::A, a);

            SetZ(false);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0x17: // rla
        {
            uint8_t a = Get(REGISTERS::A);
            uint8_t cy = GetCY() != 0 ? 0x01 : 0x00;
            uint8_t c = (a & 0b10000000);
            a = (a << 1) | cy;
            Set(REGISTERS::A, a);

            SetZ(false);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0x0F: // rrca
        {
            uint8_t a = Get(REGISTERS::A);
            uint8_t c = (a & 0b00000001) << 7;
            a = (a >> 1) | c;
            Set(REGISTERS::A, a);

            SetZ(false);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0x1F: // rra
        {
            uint8_t a = Get(REGISTERS::A);
            uint8_t cy = GetCY() != 0 ? 0x80 : 0x00;
            uint8_t c = (a & 1) << 7;
            a = (a >> 1) | cy;
            Set(REGISTERS::A, a);

            SetZ(false);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 4;
        }
            break;
        case 0xC3: // JP
        {
            uint16_t nn = ReadPC16();
            programCounter = nn;

            cycles = 12;
        }
            break;
        case 0xC2: // jpnz
        {
            uint16_t nn = ReadPC16();

            if (!GetZ()) {
                programCounter = nn;
            }

            cycles = 12;
        }
            break;
        case 0xCA: // jpz
        {
            uint16_t nn = ReadPC16();

            if (GetZ()) {
                programCounter = nn;
            }

            cycles = 12;
        }
            break;
        case 0xD2: // jpnc
        {
            uint16_t nn = ReadPC16();

            if (!GetCY()) {
                programCounter = nn;
            }

            cycles = 12;
        }
            break;
        case 0xDA: // jpc
        {
            uint16_t nn = ReadPC16();

            if (GetCY()) {
                programCounter = nn;
            }

            cycles = 12;
        }
            break;
        case 0xE9: // jphl
        {
            uint16_t nn = Get(FULL_REGISTERS::HL);
            programCounter = nn;

            cycles = 4;
        }
            break;
        case 0x18: // jr n
        {
            int8_t n = TWOS_COMPLEMENT(ReadPC());
            programCounter += n;

            cycles = 8;
        }
            break;
        case 0x20: // jr nz.n
        {
            uint8_t un = ReadPC();
            int8_t n =  TWOS_COMPLEMENT(un);

            if (!GetZ()) {
                programCounter += n;
            }

            cycles = 8;
        }
            break;
        case 0x28: // jr z.n
        {
            uint8_t un = ReadPC();
            int8_t n =  TWOS_COMPLEMENT(un);
            if (GetZ()) {
                programCounter += n;
            }

            cycles = 8;
        }
            break;
        case 0x30: // jr nc.n
        {
            uint8_t un = ReadPC();
            int8_t n =  TWOS_COMPLEMENT(un);
            if (!GetCY()) {
                programCounter += n;

            }

            cycles = 8;
        }
            break;
        case 0x38: // jr c.n
        {
            uint8_t un = ReadPC();
            int8_t n =  TWOS_COMPLEMENT(un);
            if (GetCY()) {
                programCounter += n;
            }

            cycles = 8;
        }
            break;
        case 0xCD: // CALL nn
        {
            // stackPointer--;
            // uint16_t addr = stackPointer--;
             uint16_t nn = ReadPC16();
            // mmu->Write(addr, programCounter);
            PushSP(programCounter);
            programCounter = nn;

            cycles = 12;
        }
            break;

        case 0xC4: // call nz.n
        {
            uint16_t nn = ReadPC16();
            if (!GetZ()) {
                //stackPointer--;
                //uint16_t addr = stackPointer--;
                //mmu->Write(addr, programCounter);

                PushSP(programCounter);
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xCC: // call z.n
        {
            uint16_t nn = ReadPC16();
            if (GetZ()) {
                //stackPointer--;
                //uint16_t addr = stackPointer--;
                //mmu->Write(addr, programCounter);

                PushSP(programCounter);
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xD4: // call nc.n
        {
            uint16_t nn = ReadPC16();
            if (!GetCY()) {
                //stackPointer--;
                //uint16_t addr = stackPointer--;
                //mmu->Write(addr, programCounter);

                PushSP(programCounter);
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xDC: // call c.n
        {
            uint16_t nn = ReadPC16();
            if (GetCY()) {
                //stackPointer--;
                //uint16_t addr = stackPointer--;
                //mmu->Write(addr, programCounter);

                PushSP(programCounter);
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xC7: case 0xCF: case 0xD7: case 0xDF: case 0xE7: case 0xEF: case 0xF7: case 0xFF: // RST n
        {
            //stackPointer--;
            //uint16_t addr = stackPointer--;
            //mmu->Write(addr, programCounter);

            PushSP(programCounter);
            programCounter = 0x0000 | (opcode - 0xC7);

            cycles = 32;
        }
            break;
        case 0xC9: // RET
        {
            //uint16_t addr = stackPointer++;
            //stackPointer++;
            //uint16_t nn = mmu->Read16Bit(addr);
            uint16_t nn = PopSP16();
            programCounter = nn;

            cycles = 8;
        }
            break;
        case 0xC0: // RET cc
        {
            if (!GetZ()) {
                //uint16_t addr = stackPointer++;
                //stackPointer++;
                //uint16_t nn = mmu->Read16Bit(addr);
                uint16_t nn = PopSP16();
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xC8: // RET cc
        {
            if (GetZ()) {
                //uint16_t addr = stackPointer++;
                //stackPointer++;
                //uint16_t nn = mmu->Read16Bit(addr);
                uint16_t nn = PopSP16();
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xD0: // RET cc
        {
            if (!GetCY()) {
                //uint16_t addr = stackPointer++;
                //stackPointer++;
                //uint16_t nn = mmu->Read16Bit(addr);
                uint16_t nn = PopSP16();
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xD8: // RET cc
        {
            if (GetCY()) {
                //uint16_t addr = stackPointer++;
                //stackPointer++;
                //uint16_t nn = mmu->Read16Bit(addr);
                uint16_t nn = PopSP16();
                programCounter = nn;
            }

            cycles = 8;
        }
            break;
        case 0xD9: // RETI
        {
            //uint16_t addr = stackPointer++;
            //stackPointer++;
            //uint16_t nn = mmu->Read16Bit(addr);
            IME = 0xFF;
            uint16_t nn = PopSP16();
            programCounter = nn;

            cycles = 8;
        }
            break;
        default:
        {
            fprintf(stderr,"==== WARNING: OPCODE: 0x%.2X does not exist on line %.4X. ====\n", opcode, programCounter);
        }
            break;
    }
}

void CPU::CheckExtension(uint8_t opcode) {
    // gpu->Print(8,0,debugBuffer);
    switch (opcode) {
        case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x37: // swap n
        {
            REGISTERS r = RegisterParse(opcode);
            uint8_t old =  Get(r);
            uint8_t newHi = old & 0x0F << 4;
            uint8_t newLo = old & 0xF0 >> 4;
            uint8_t swap = newHi | newLo;
            Set(r, swap);

            SetZ(0 == swap);
            SetN(false);
            SetCY(false);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x36: // swap (hl)
        {
            uint16_t addr =  Get(FULL_REGISTERS::HL);
            uint8_t old = mmu->Read(addr);
            uint8_t newHi = old & 0x0F << 4;
            uint8_t newLo = old & 0xF0 >> 4;
            uint8_t swap = newHi | newLo;
            mmu->Write(addr, swap);

            SetZ(0 == swap);
            SetN(false);
            SetCY(false);
            SetH(false);

            cycles = 16;
        }
            break;
            /* RLC */
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: // rlc reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);
            uint8_t a = Get(r);
            uint8_t c = a >> 7;
            a = (a << 1) | c;
            Set(r, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x06: // rlc (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t a = mmu->Read(addr);
            uint8_t c = a >> 7;
            a = (a << 1) | c;
            mmu->Write(addr, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 16;
        }
            break;
            /* RL */
        case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x17: // rl reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);

            uint8_t a = Get(r);
            uint8_t cy = GetCY() != 0 ? 1 : 0;
            uint8_t c = a >> 7;
            a = (a << 1) | cy;
            Set(r, a);
            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x16: // rl (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t a = mmu->Read(addr);
            uint8_t cy = GetCY() != 0 ? 0b00000001 : 0b00000000;
            uint8_t c = a >> 7;
            a = (a << 1) | cy;
            mmu->Write(addr, a);

            SetZ(a);
            SetN(false);
            SetCY(c);
            SetH(false);

            cycles = 16;
        }
            break;
            /* RRC */

        case 0x08: case 0x09: case 0x0A: case 0x0B: case 0x0C: case 0x0D: case 0x0F: // rrc reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);
            uint8_t a = Get(r);
            uint8_t c = (a & 0b00000001) << 7;
            a = (a >> 1) | c;
            Set(r, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x0E: // rlc (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t a = mmu->Read(addr);
            uint8_t c = (a & 0b00000001) << 7;
            a = (a >> 1) | c;
            mmu->Write(addr, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 16;
        }
            break;
            /* RR */
        case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D: case 0x1F: // rr reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);

            uint8_t a = Get(r);
            uint8_t cy = GetCY() != 0 ? 0x80 : 0x00;
            uint8_t c = (a & 0b00000001) << 7;
            a = (a >> 1) | cy;
            Set(r, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x1E: // rr (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t a = mmu->Read(addr);
            uint8_t cy = GetCY() != 0 ? 0x80 : 0x00;
            uint8_t c = (a & 0b00000001) << 7;
            a = (a >> 1) | cy;
            mmu->Write(addr, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 16;
        }
            break;

        /* SLA n */
        case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x27: // sla reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);

            uint8_t a = Get(r);
            uint8_t c = a >> 7;
            a = (a << 1);
            Set(r, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x26: // sla (hl)
        {

            uint16_t addr = Get(FULL_REGISTERS::HL);

            uint8_t a = mmu->Read(addr);
            uint8_t c = a >> 7;
            a = (a << 1);
            mmu->Write(addr, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 16;
        }
            break;

        /* SRA n */
        case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D: case 0x2F: // sra reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);

            uint8_t a = Get(r);
            uint8_t msb = a & 0b10000000;
            uint8_t c = a & 0b00000001;
            a = (a >> 1) | msb;
            Set(r, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x2E: // sra (hl)
        {

            uint16_t addr = Get(FULL_REGISTERS::HL);

            uint8_t a = mmu->Read(addr);
            uint8_t msb = a & 0b10000000;
            uint8_t c = a & 0b00000001;
            a = (a >> 1) | msb;
            mmu->Write(addr, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 16;
        }
            break;

        /* SRL n */
        case 0x38: case 0x39: case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3F: // srl reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);

            uint8_t a = Get(r);
            uint8_t c = a & 0b00000001;
            a = (a >> 1);
            Set(r, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 8;
        }
            break;
        case 0x3E: // sra (hl)
        {

            uint16_t addr = Get(FULL_REGISTERS::HL);

            uint8_t a = mmu->Read(addr);
            uint8_t c = a & 0b00000001;
            a = (a >> 1);
            mmu->Write(addr, a);

            SetZ(a == 0);
            SetN(false);
            SetCY(c != 0);
            SetH(false);

            cycles = 16;
        }
            break;

        /* BIT b,r */
        case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x47: // bit 0, reg
        case 0x48: case 0x49: case 0x4A: case 0x4B: case 0x4C: case 0x4D: case 0x4F: // bit 1, reg
        case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x57: // bit 2, reg
        case 0x58: case 0x59: case 0x5A: case 0x5B: case 0x5C: case 0x5D: case 0x5F: // bit 3, reg
        case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x67: // bit 4, reg
        case 0x68: case 0x69: case 0x6A: case 0x6B: case 0x6C: case 0x6D: case 0x6F: // bit 5, reg
        case 0x70: case 0x71: case 0x72: case 0x73: case 0x74: case 0x75: case 0x77: // bit 6, reg
        case 0x78: case 0x79: case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7F: // bit 7, reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);
            uint8_t bit = (opcode - 0x40) >> 3;
            uint8_t b = 1 << bit;

            uint8_t reg = Get(r);
            uint8_t result = reg & b;

            SetZ(0 == result);

            SetN(false);
            SetH(true);

            cycles = 8;
            break;
        }
            break;
        case 0x46: // bit 0, (hl)
        case 0x4E: // bit 1, (hl)
        case 0x56: // bit 2, (hl)
        case 0x5E: // bit 3, (hl)
        case 0x66: // bit 4, (hl)
        case 0x6E: // bit 5, (hl)
        case 0x76: // bit 6, (hl)
        case 0x7E: // bit 7, (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t bit = (opcode - 0x46) >> 3;
            uint8_t b = (1 << bit);
            uint8_t mem = mmu->Read(addr);

            uint8_t result = mem & b;

            SetZ(result == 0);

            SetN(false);
            SetH(true);

            cycles = 16;
        }
            break;

        /* SET b,r */
        case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC7: // SET 0, reg
        case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCF: // SET 1, reg
        case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD7: // SET 2, reg
        case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDF: // SET 3, reg
        case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE7: // SET 4, reg
        case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEF: // SET 5, reg
        case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7: // SET 6, reg
        case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFF: // SET 7, reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);
            uint8_t bit = (opcode - 0xC0)  >> 3;
            uint8_t b = 1 << bit;

            uint8_t val = Get(r);
            val = val | b;
            Set(r, val);

            cycles = 8;
        }
            break;
        case 0xC6: // SET 0, (hl)
        case 0xCE: // SET 1, (hl)
        case 0xD6: // SET 2, (hl)
        case 0xDE: // SET 3, (hl)
        case 0xE6: // SET 4, (hl)
        case 0xEE: // SET 5, (hl)
        case 0xF6: // SET 6, (hl)
        case 0xFE: // SET 7, (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t bit = (opcode - 0xC6)  >> 3;
            uint8_t b = 1 << bit;

            uint8_t mem = mmu->Read(addr);
            uint8_t result = mem | b;

            mmu->Write(addr, result);

            cycles = 16;
        }
            break;

        /* RES b,r */
        case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x87: // RES 0, reg
        case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8F: // RES 1, reg
        case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x97: // RES 2, reg
        case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9F: // RES 3, reg
        case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA7: // RES 4, reg
        case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAF: // RES 5, reg
        case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB7: // RES 6, reg
        case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBF: // RES 7, reg
        {
            REGISTERS r = (REGISTERS)(opcode & 0b0111);
            uint8_t bit = (opcode - 0x80)  >> 3;
            uint8_t b = 1 << bit;
            b = ~b;

            uint8_t val = Get(r);
            uint8_t result = (val & b);
            // fprintf(stderr, "OP: 0xCB%.2X   preval: %d   postval: %d   F: 0x%.2X\n", opcode, val,result,Get(REGISTERS::F));
            Set(r, result);

            cycles = 8;
        }
            break;
        case 0x86: // RES 0, (hl)
        case 0x8E: // RES 1, (hl)
        case 0x96: // RES 2, (hl)
        case 0x9E: // RES 3, (hl)
        case 0xA6: // RES 4, (hl)
        case 0xAE: // RES 5, (hl)
        case 0xB6: // RES 6, (hl)
        case 0xBE: // RES 7, (hl)
        {
            uint16_t addr = Get(FULL_REGISTERS::HL);
            uint8_t bit = (opcode - 0x86) >> 3;
            uint8_t b = 1 << bit;
            b = ~b;

            uint8_t mem = mmu->Read(addr);
            uint8_t result = mem & b;
            mmu->Write(addr, result);

            cycles = 16;
        }
            break;
        default:
        {
            fprintf(stderr,"==== WARNING: OPCODE: 0xCB%.2X does not exist on line %.4X. ====\n", opcode, programCounter);
        }
            break;
    }
}


void CPU::CheckRegisters() {
    if(Get(REGISTERS::A) != 0x01) { std::cout << "Failed on register a" << std::endl; exit(1); }
    if(Get(REGISTERS::B) != 0x00) { std::cout << "Failed on register b" << std::endl; exit(1); }
    if(Get(REGISTERS::C) != 0x13) { std::cout << "Failed on register c" << std::endl; exit(1); }
    if(Get(REGISTERS::D) != 0x00) { std::cout << "Failed on register d" << std::endl; exit(1); }
    if(Get(REGISTERS::E) != 0xD8) { std::cout << "Failed on register e" << std::endl; exit(1); }
    if(Get(REGISTERS::H) != 0x01) { std::cout << "Failed on register h" << std::endl; exit(1); }
    if(Get(REGISTERS::L) != 0x4D) { std::cout << "Failed on register l" << std::endl; exit(1); }
    if(stackPointer != 0xFFFE) { std::cout << "Failed on registers sp" << std::endl; exit(1); }
    if(programCounter != 0x0100) { std::cout << "Failed on registers pc" << std::endl; exit(1); }

    if (GetZ() != 0x1) { std::cout << "Failed on flag Z" << std::endl; exit(1); }
    if (GetN() != 0x0) { std::cout << "Failed on flag N" << std::endl; exit(1); }
    if (GetH() != 0x1) { std::cout << "Failed on flag H" << std::endl; exit(1); }
    if (GetCY() != 0x1) { std::cout << "Failed on flag CY" << std::endl; exit(1); }
}
