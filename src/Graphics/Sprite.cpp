uint8_t spritelineCount = 0;
for (int ii = 0; ii < 40; ii++) {
    int8_t spriteY = mmu->sprites[ii][0];
    int8_t spriteX = mmu->sprites[ii][1];
    uint8_t spriteIndex = mmu->sprites[ii][2];
    uint8_t spriteData = mmu->sprites[ii][3];

    // meta data broken out
    bool priority = (spriteData & Utils::BIT_7) != 0;
    bool flipY    = (spriteData & Utils::BIT_6) != 0;
    bool flipX    = (spriteData & Utils::BIT_5) != 0;
    uint8_t obp   = (spriteData & Utils::BIT_4) >> 4;

    // does this sprite exist on our scanline
    // the weird less than AND greater than + 8 is relevant for the flips
    if ((spriteY <= scanline) && ((spriteY + 8) > scanline)) {
        size_t canvasOffset = (scanline * MAX_X + spriteX);

        // the data for this line of the sprite
        uint8_t y = (( scanline - spriteY ) & 7);

        // if flipped vertically, use the opposite side of the tile
        if (flipY) {
            y = (7 - y);
        }


        // where to render on canvas
        uint8_t color;
        uint8_t x;

        for (int ii = 0; ii < 8; ii++) {
            x = (flipX ? (7 - ii) : ii);
            color = mmu->tiles[spriteIndex][y][x];
            // color = mmu->obp[obp][color];

            if (
                // if pixel is on screen AND
                (((spriteX + ii) >= 0) && ((spriteX + ii) < MAX_X)) &&
                // if pixel is not color == 0 AND
                (color != 0) &&
                // if pixel has priority OR bg color == 0
                ((priority) || (framebuffer[canvasOffset] == 0))
            ) {
                // store the pixel into the framebuffer

                color = mmu->obp[obp][color];
                framebuffer[canvasOffset] = color;
                canvasOffset++;
            }
        }

        spritelineCount++;

        if (spritelineCount >= 10) {
            break;
        }
    }
}
