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

int main(int argc, char* argv[]) {
    Cartridge* cart = new Cartridge("carts/cpu_instrs.gb");
    if (!cart->IsLoaded()) {
        std::cout << "Cartridge is not loaded." << std::endl;
        exit(1);
    }

    MMU* mmu = new MMU(cart);
    CPU* cpu = new CPU(mmu);
    // InterruptService* is = new InterruptService(cpu, mmu);
    // iGPU* gpu = new TextGPU(mmu);

    uint32_t counter = 0;
    uint32_t breakpointExtension = 0;
    while (true) {
        cpu->Read(); // fetch, decode, execute

        //is->CheckInterrupts();

        //gpu->Step(cpu->GetCycles());

        if (0 < breakpointExtension && 50 > breakpointExtension) {
          std::cout << "Breakpoint" << std::endl;
        }

        if (0x0100 == cpu->programCounter) {
            cpu->CheckRegisters();
            mmu->CheckMemory();

            //gpu->DumpTileset();
        }

        counter++;
    }

    return 0;
}
