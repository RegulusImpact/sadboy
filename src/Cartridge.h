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
    uint8_t* rom;
    bool fileLoaded;
    uint32_t fileSize;

public:
    Cartridge(std::string fileName);
    ~Cartridge();

    uint8_t Read(uint32_t addr);
    uint32_t GetFileSize() { return fileSize; }
    bool IsLoaded() { return fileLoaded; }
};

#endif
