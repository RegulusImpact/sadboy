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
#include "Graphics/TextGPU.h"

int main(int argc, char* argv[]) {
    Cartridge* cart = new Cartridge("carts/cpu_instrs.gb");
    if (!cart->IsLoaded()) {
        std::cout << "Cartridge is not loaded." << std::endl;
        exit(1);
    }
    
    MMU* mmu = new MMU(cart);
    CPU* cpu = new CPU(mmu);
    InterruptService* is = new InterruptService(cpu, mmu);
    iGPU* gpu = new TextGPU(mmu);
    clear();
    //ScreenSaver(gpu);
    bool bp = false;
    
    uint32_t dot = 0, lin = 0;
    uint32_t counter = 0;
    uint32_t breakpointExtension = 0;
    uint16_t maxpc = 0;
    while (true) {
        cpu->Read(); // fetch, decode, execute
        
        //is->CheckInterrupts();
        
        gpu->Step(cpu->GetCycles());
        
        
        // if (counter % 500000 == 0) {
        //     if (lin == 14) {
        //         dot = 0;
        //         lin = 0;
        //         clear();
        //     }
            
        //     mvprintw(lin, dot++, ".");
        //     mvprintw(0, 51, "MAX: %.4X", maxpc);
            
        //     if (dot > 50) {
        //         dot = 0;
        //         lin++;
        //     }
        //     refresh();
        // }
        
        if (0x0100 == cpu->programCounter) {
            cpu->CheckRegisters();
            mmu->CheckMemory();
            
            gpu->DumpTileset();
        }
        
        counter++;
    }
    
    return 0;
}