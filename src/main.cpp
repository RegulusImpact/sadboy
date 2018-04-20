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
#include "MBC.h"

#include "Graphics/iGPU.h"
#include "Graphics/XGPU.h"

int main() {
    /* BLARGGS CPU INSTRUCTION TESTS */
    MBC* mbc = new MBC();
    // mbc->LoadCart();

    MMU* mmu = new MMU(mbc);
    CPU* cpu = new CPU(mmu);
    InterruptService* is = new InterruptService(cpu, mmu);
    TimerService* ts = new TimerService(cpu, mmu, is);
    iGPU* gpu = new XGPU(mmu, 3);
    printf("Setup Complete:\n");

    uint32_t counter = 0;

    while (counter < 10) {
        printf("cpu->programCounter: %d\n",cpu->programCounter);
        bool dispatcher = is->CheckInterrupts();

        if (dispatcher) {
            is->Dispatch();
        }

        cpu->Read(); // fetch, decode, execute
        // ts->Increment();

        gpu->Step(cpu->GetCycles());
        ts->Increment();

        if (0x0100 == cpu->programCounter && mmu->readBios) {
            cpu->CheckRegisters();
            mmu->CheckMemory();
            mmu->readBios = false;
        }

        counter++;
    }

    return 0;
}
