// Cartridge.cpp

#include "Cartridge.h"

Cartridge::Cartridge(std::string file) {
    filePath = file;
    maxRamSize = 0;
    LoadFile();
    if (fileLoaded) {
        LoadHeader();
        CreateRAM();
    }
}

Cartridge::~Cartridge() {
    delete[] rom;
}

void Cartridge::LoadFile() {
    std::FILE* file;
    file = fopen (filePath.c_str() , "rb");
    if (file==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (file , 0 , SEEK_END);
    fileSize = ftell (file);
    rewind (file);

    // allocate memory to contain the whole file:
    rom = new uint8_t[fileSize];
    if (rom == NULL) {fputs ("Memory error",stderr); exit (2);}

    // copy the file into the buffer:
    uint32_t result = fread (rom,1,fileSize,file);
    if (result != fileSize) {fputs ("Reading error",stderr); exit (3);}
    fclose (file);

    fileLoaded = true;
}

void Cartridge::LoadHeader() {
    /*
    // Cart data
    char title[16]; // 0x134 - 0x143
    bool sgbFlag; // 0x146
    uint8_t cartType; // 0x147
    uint8_t romSize; // 0x148
    uint8_t ramSize; // 0x149
    bool destinationCode; // 0x14A
    */

    for (size_t ii = 0x134; ii <= 0x143; ii++) {
        title[ii - 0x134] = (char)(Read(ii));
    }

    sgbFlag = 0x00 != Read(0x146);
    cartType = Read(0x147);
    romSize = Read(0x148);
    ramSize = Read(0x149);
    destinationCode = 0 != Read(0x14A);
}

void Cartridge::CreateRAM() {
    uint32_t bankCount = 0;
    uint32_t ramBankSize = 0;

    switch (ramSize) {
        case 00: { // no ram
            bankCount = 0;
            ramBankSize = 0;
        }
            break;
        case 01: { // 2KB
            bankCount = 1;
            ramBankSize = 0x0800;
        }
            break;
        case 02: { // 8KB
            bankCount = 1;
            ramBankSize = 0x2000;
        }
            break;
        case 03: { // 32KB (4 of 8KB)
            bankCount = 4;
            ramBankSize = 0x2000;
        }
            break;
        case 04: { // 128KB (16 of 8KB)
            bankCount = 16;
            ramBankSize = 0x2000;
        }
            break;
        case 05: { // 64KB (8 of 8KB)
            bankCount = 8;
            ramBankSize = 0x2000;
        }
            break;
    }
    maxRamSize = bankCount * ramBankSize;
    if (maxRamSize != 0) {
        ram = new uint8_t[maxRamSize];
    }
}

uint8_t Cartridge::Read(uint32_t addr) {
    if (rom != NULL && addr < fileSize) {
        return (uint8_t)rom[addr];
    }

    return 0x00;
}
uint8_t Cartridge::ReadRAM(uint32_t addr) {
    if (ram != NULL && addr < maxRamSize) {
        return (uint8_t)ram[addr];
    }

    return 0x00;
}
void Cartridge::WriteRAM(uint32_t addr, uint8_t val) {
    if (ram != NULL && addr < maxRamSize) {
        ram[addr] = val;
    }
}

bool Cartridge::HeaderChecksum() {
    uint8_t checksum = 0;

    for (size_t ii = 0x134; ii <= 0x14C; ii++) {
        checksum = checksum - Read(ii) - 1;
    }

    printf("ACTUAL: %d | EXPECTED: %d\n", checksum, Read(0x014D));

    return checksum == Read(0x014D);
}

void Cartridge::DumpInfo() {
    printf("Cartridge Details:\n");
    printf("\t Title: %s\n", title);
    printf("\t SGB Features Supported: %s\n", sgbFlag ? "True" : "False");
    printf("\t Cartridge Type: 0x%.4X\n", cartType);
    printf("\t ROM Size: 0x%.4X\n", romSize);
    printf("\t RAM Size: 0x%.4X\n", ramSize);
    printf("\t Destination Code: %s\n", destinationCode ? "WW" : "JP");
}
