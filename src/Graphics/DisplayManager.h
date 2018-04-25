#ifndef DisplayManager_H
#define DisplayManager_H

// uintX_t
#include <cstdint>
// size_t
#include <cstddef>

// X11 Support
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "../MMU.h"
#include "../Utils.h"

class DisplayManager {
private:
    // Display and window configurations
    Display* display;
    int screen;
    Window window;
    uint8_t windowScalar;

    // Keyinput
    int x11_fd; // connection number, needed to short circuit nextEvent blockign
    fd_set in_fds;

    struct timeval tv;
    XEvent event;

    // Graphics
    GC gc;
    Colormap cmap;
    uint32_t black, white;
    XColor palette[4];

    void init_x();
    void close_x();
    void init_palette(MMU* mmu);

public:
    DisplayManager(MMU* mmu, uint8_t ws);
    ~DisplayManager();

    void Draw(uint8_t color, uint8_t y, uint8_t x);
    void Draw(XColor color, uint8_t y, uint8_t x);

    uint16_t GetKeyInput();
};

#endif
