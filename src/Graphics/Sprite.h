// Sprite.h
#ifndef Sprite_H
#define Sprite_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>
#include <iostream>

class Sprite {
private:
    uint8_t palette;
    int16_t x;
    int16_t y;
    uint32_t tile;

    bool priority;
    bool flipX;
    bool flipY;

public:
    Sprite();
    Sprite(int ii);

    /* Getters */
    uint8_t GetPalette() { return palette; }
    int16_t GetX() { return x; }
    int16_t GetY() { return y; }
    uint32_t GetTile() { return tile; }

    bool GetPriority() { return priority; }
    bool GetFlipX() { return flipX; }
    bool GetFlipY() { return flipY; }

    /* Setters */
    void SetPalette(uint8_t val) { palette = val; }
    void SetX(int16_t val) { x = val; }
    void SetY(int16_t val) { y = val; }
    void SetTile(uint32_t val) { tile = val; }

    void SetPriority(bool val) { priority = val; }
    void SetFlipX(bool val) { flipX = val; }
    void SetFlipY(bool val) { flipY = val; }

    void PrettyPrint() {
        printf("\tSprite:\n");
        printf("\t\ttile[x%.4X] @ (%d,%d) with pal[%d]\n", tile,x,y,palette);
        printf("\t\tflipx: %d | flipy: %d | prio: %d\n", flipX, flipY, priority);
    }
};

#endif
