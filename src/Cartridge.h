// Cartridge.h
#ifndef Cartridge_H
#define Cartridge_H

#include <iostream>
#include <fstream>
// uintX_t
#include <cstdint>
// size_t
#include <cstddef>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

class Cartridge {
private:
    std::string filePath;
    uint8_t* rom;
    uint8_t* ram;
    bool fileLoaded;
    uint32_t fileSize;
    uint32_t maxRamSize;



public:
    // Cart data
    char title[16]; // 0x134 - 0x143
    bool sgbFlag; // 0x146
    uint16_t cartType; // 0x147
    uint16_t romSize; // 0x148
    uint16_t ramSize; // 0x149
    bool destinationCode; // 0x14A - true = nonJP

    Cartridge(std::string fileName);

    void LoadFile();
    void LoadHeader();
    void CreateRAM();
    bool HeaderChecksum();

    uint8_t Read(uint32_t addr);
    uint8_t ReadRAM(uint32_t addr);
    void WriteRAM(uint32_t addr, uint8_t val);

    void DumpInfo(); // pretty print

    uint32_t GetFileSize() { return fileSize; }
    bool IsLoaded() { return fileLoaded; }
};

#endif
