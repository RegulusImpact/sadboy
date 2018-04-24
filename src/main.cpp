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
    Cartridge* cart;
    /* BLARGGS CPU INSTRUCTION TESTS */
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/01-special.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb"); // -- Passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"); // -- Passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/05-op rp.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/09-op r,r.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/10-bit ops.gb"); // -- passed
    // cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb"); // -- passed

    /* BLARGGS TIMING TESTS */
    // cart = new Cartridge("submodules/gb-test-roms/interrupt_time/interrupt_time.gb"); // -- failed
    // cart = new Cartridge("submodules/gb-test-roms/instr_timing/instr_timing.gb"); // -- failed
    // cart = new Cartridge("submodules/gb-test-roms/mem_timing/mem_timing.gb"); // -- failed
    // cart = new Cartridge("submodules/gb-test-roms/mem_timing/individual/01-read_timing.gb");

    // The holy grail of tests :D
    // cart = new Cartridge("/home/regulus/github/sadboy/test/div_write.gb"); // -- failed
    // cart = new Cartridge("/home/regulus/github/sadboy/test/ie_push.gb"); // -- failed
    // cart = new Cartridge("/home/regulus/github/sadboy/test/rapid_toggle.gb"); // -- failed
    cart = new Cartridge("/home/regulus/github/java-gb/src/main/resources/tetris.gb"); // -- failed
    // cart = new Cartridge("/home/regulus/github/java-gb/src/main/resources/pokebluejp.gb"); // -- failed
    // cart = new Cartridge("/home/regulus/github/java-gb/src/main/resources/drmario.gb");
    // cart = new Cartridge("/home/regulus/Downloads/reg_f.gb"); // -- failed
    // cart = new Cartridge("/home/regulus/Downloads/unused_hwio-GS.gb"); // -- failed

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
    while (true) {
        // is->CheckInterrupts();
        cpu->Read(); // fetch, decode, execute

        gpu->Step(cpu->GetCycles());
        // ts->Increment();

        if (0x0100 == cpu->programCounter && mmu->readBios) {
            cpu->CheckRegisters();
            mmu->CheckMemory();
            mmu->readBios = false;
        }

        counter++;
    }

    return 0;
}
