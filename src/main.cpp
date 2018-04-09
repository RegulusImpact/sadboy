#include <iostream>
// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

#include <unistd.h>

#include <string>
#include <random>

#include "CPU.h"
#include "MMU.h"
#include "InterruptService.h"
#include "Cartridge.h"
#include "Graphics/iGPU.h"
#include "Graphics/XGPU.h"

int main(int argc, char* argv[]) {
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/01-special.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/05-op rp.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb"); // -- failed
    Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/09-op r,r.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/10-bit ops.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb"); // -- failed
    if (!cart->IsLoaded()) {
        std::cout << "Cartridge is not loaded." << std::endl;
        exit(1);
    }

    MMU* mmu = new MMU(cart);
    CPU* cpu = new CPU(mmu);
    // InterruptService* is = new InterruptService(cpu, mmu);
    // iGPU* gpu = new TextGPU(mmu);
    iGPU* gpu = new XGPU(mmu, 3);

    uint32_t counter = 0;
    uint32_t breakpointExtension = 0;
    while (true) {
        cpu->Read(); // fetch, decode, execute

        //is->CheckInterrupts();
        // usleep(350);

        gpu->Step(cpu->GetCycles());

        if (0 < breakpointExtension && 50 > breakpointExtension) {
          std::cout << "Breakpoint" << std::endl;
        }

        if (0x0100 == cpu->programCounter) {
            cpu->CheckRegisters();
            mmu->CheckMemory();

            // gpu->DumpTileset();
            // gpu->DumpTiles();
        }

        counter++;
    }

    return 0;
}
