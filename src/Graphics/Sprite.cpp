//Sprite.cpp
#include "Sprite.h"

Sprite::Sprite() {
    palette = 0; // defaulting everything out
    x = 0; // defaulting everything out
    y = 0; // defaulting everything out
    tile = 0; // defaulting everything out

    priority = false; // defaulting everything out
    flipX = false; // defaulting everything out
    flipY = false; // defaulting everything out
}


Sprite::Sprite(int ii) {
    palette = 0; // defaulting everything out
    x = ii; // defaulting everything out
    y = ii; // defaulting everything out
    tile = 0; // defaulting everything out

    priority = false; // defaulting everything out
    flipX = false; // defaulting everything out
    flipY = false; // defaulting everything out
}
