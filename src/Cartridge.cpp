// Cartridge.cpp

#include "Cartridge.h"


/*

nt main () {
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( "myfile.bin" , "rb" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

   the whole file is now loaded in the memory buffer.

  // terminate
  fclose (pFile);
  free (buffer);
  return 0;
}

*/
Cartridge::Cartridge(std::string fileName) {
    std::FILE* file;
    file = fopen (fileName.c_str() , "rb");
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

    printf("File loaded: %s\n", fileName.c_str());

    fileLoaded = true;
}

Cartridge::~Cartridge() {
    delete[] rom;
}

uint8_t Cartridge::Read(uint32_t addr) {
    return (uint8_t)rom[addr];
}
