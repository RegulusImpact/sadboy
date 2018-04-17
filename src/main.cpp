#include <iostream>
// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

#include <unistd.h>

#include <string>
#include <random>

#include <chrono>

#include "CPU.h"
#include "MMU.h"
#include "InterruptService.h"
#include "TimerService.h"

#include "Cartridge.h"

#include "Graphics/iGPU.h"
#include "Graphics/XGPU.h"

int main() {
    /* BLARGGS CPU INSTRUCTION TESTS */
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/01-special.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb"); // -- Passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"); // -- Passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/05-op rp.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/09-op r,r.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/10-bit ops.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb"); // -- passed

    /* BLARGGS TIMING TESTS */
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/interrupt_time/interrupt_time.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/instr_timing/instr_timing.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/mem_timing/mem_timing.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/mem_timing/individual/01-read_timing.gb");

    // The holy grail of tests :D
    // Cartridge* cart = new Cartridge("/home/regulus/github/sadboy/test/div_write.gb"); // -- failed
    // Cartridge* cart = new Cartridge("/home/regulus/github/sadboy/test/ie_push.gb"); // -- failed
    // Cartridge* cart = new Cartridge("/home/regulus/github/sadboy/test/rapid_toggle.gb"); // -- failed
    Cartridge* cart = new Cartridge("/home/regulus/github/java-gb/src/main/resources/tetris.gb"); // -- failed
    if (!cart->IsLoaded()) {
        std::cout << "Cartridge is not loaded." << std::endl;
        exit(1);
    }

    MMU* mmu = new MMU(cart);
    CPU* cpu = new CPU(mmu);
    InterruptService* is = new InterruptService(cpu, mmu);
    TimerService* ts = new TimerService(cpu, mmu, is);
    iGPU* gpu = new XGPU(mmu, 3);

    uint32_t counter = 0;
    uint32_t breakpointExtension = 0;
    int qq;

    while (true) {
        bool dispatcher = is->CheckInterrupts();

                ts->Increment();
        if (dispatcher) {
            is->Dispatch();
        } else {
            cpu->Read(); // fetch, decode, execute
        }
                ts->Increment();

        gpu->Step(cpu->GetCycles());

        if (0 < breakpointExtension && 50 > breakpointExtension) {
            std::cout << "Breakpoint" << std::endl;
            printf("\tAF=%.4X", cpu->Get(FULL_REGISTERS::AF));
            printf("\tBC=%.4X", cpu->Get(FULL_REGISTERS::BC));
            printf("\tDE=%.4X", cpu->Get(FULL_REGISTERS::DE));
            printf("\tHL=%.4X", cpu->Get(FULL_REGISTERS::HL));
            printf("\tSP=%.4X", cpu->stackPointer           );
            printf("\tPC=%.4X", cpu->programCounter         );
            std::cin >> qq;
            breakpointExtension--;
        }

        if (0x0100 == cpu->programCounter && mmu->readBios) {
            cpu->CheckRegisters();
            mmu->CheckMemory();
            mmu->readBios = false;
        }

        counter++;
    }

    return 0;
}
