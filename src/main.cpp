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

int main() {

    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/01-special.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/02-interrupts.gb"); // -- failed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb"); // -- failed 88
    Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb"); // -- failed EE
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/05-op rp.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/09-op r,r.gb"); // -- failed 88 99 aa bb cc dd ff 00 11 22 33 44 55 77
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/10-bit ops.gb"); // -- passed
    // Cartridge* cart = new Cartridge("submodules/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb"); // -- failed EE 66BB
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
    int qq;

    while (true) {
        cpu->Read(); // fetch, decode, execute


        // by c025, the title prints

        // by c7ef shits broke
        //    f is wrong
        //         because add hlhl is wrong

        //code enters infinite loop somewhere in the call c787,
        // never reaches cb2b

        // throwing no opcde on "def9"

        // c4af
        // c4 e6

        //c4e6

        // //09

        // breaks somewhere in the function call at cadd
        // breaks somewhere in ca35 as this leads to cadd
        // breaks somewhere in ca97
        // "        "           c9f2
        // if (cpu->programCounter == 0xCA01) {
        // if (cpu->programCounter == 0xCA01) {
        //     breakpointExtension = 1;
        // }

        //is->CheckInterrupts();
        // usleep(350);
        // if (f == 0x00 && ap == 0x14 && af == 0x34) {
        //     breakpointExtension = 1;
        // }

        gpu->Step(cpu->GetCycles());
        //
        // if (counter == 2368556) {
        //     breakpointExtension = 1;
        // }
        //
        //
        // if (cpu->programCounter > 0x100) {
        //     if (cpu->programCounter % 1 == 0) {
        //         fprintf(stderr,"af=%.4x bc=%.4x de=%.4x pc=%.4x\n",
        //         cpu->Get(FULL_REGISTERS::AF),
        //         cpu->Get(FULL_REGISTERS::BC),
        //         cpu->Get(FULL_REGISTERS::DE),
        //         cpu->programCounter);
        //     }
        // }

        if (0 < breakpointExtension && 50 > breakpointExtension) {
            std::cout << "Breakpoint" << std::endl;
            printf("\tAF=%.4X\t\t(DEF8): %.2X\n", cpu->Get(FULL_REGISTERS::AF),mmu->Read(0xFF40));
            printf("\tBC=%.4X\t\t(DEF9): %.2X\n", cpu->Get(FULL_REGISTERS::BC),mmu->Read(0xFF40));
            printf("\tDE=%.4X\t\t(DEFA): %.2X\n", cpu->Get(FULL_REGISTERS::DE),mmu->Read(0xFF40));
            printf("\tHL=%.4X\t\t(DEFB): %.2X\n", cpu->Get(FULL_REGISTERS::HL),mmu->Read(0xFF40));
            printf("\tSP=%.4X\t\t(DEFC): %.2X\n", cpu->stackPointer           ,mmu->Read(0xFF40));
            printf("\tPC=%.4X\t\t(DEFD): %.2X\n", cpu->programCounter         ,mmu->Read(0xFF40));
            std::cin >> qq;
            breakpointExtension--;
        }

        if (0x0100 == cpu->programCounter && mmu->readBios) {
            cpu->CheckRegisters();
            mmu->CheckMemory();
            mmu->readBios = false;

            // gpu->DumpTileset();
            // gpu->DumpTiles();
        }

        counter++;
    }

    return 0;
}
