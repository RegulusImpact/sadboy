// Sprite.h
#ifndef Sprite_H
#define Sprite_H

// io
#include <iostream>
// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

class Sprite {
private:
    std::uint8_t posX;
    std::uint8_t posY;
    std::uint8_t tile;
    
    std::uint8_t priority;
    
    bool flipX;
    bool flipY;
    
    std::uint8_t palette;
    
public:
    Sprite();
    ~Sprite();
}

#endif