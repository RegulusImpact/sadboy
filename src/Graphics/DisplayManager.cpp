// DisplayManager.cpp
#include "DisplayManager.h"

void DisplayManager::init_x() {
    // setup up the display variables
    display = XOpenDisplay((char*)0);

    if (display == NULL) {
        std::cerr << "X could not open display. Make sure the DISPLAY environment variable is configured." << std::endl;
        std::cerr << "E.g. DISPLAY:0" << std::endl;
        exit(1);
    }

    screen = XDefaultScreen(display);

    black = BlackPixel(display, screen);
    white = WhitePixel(display, screen);

    // create the window
    window = XCreateSimpleWindow(
        display,
        DefaultRootWindow(display),
        0,
        0,
        (windowScalar * Utils::MAX_X), // 200 wide
        (windowScalar * Utils::MAX_Y), // 300 down
        5,
        white, // foreground white
        black // background black
    );

    // set window properties
    XSetStandardProperties(
        display,
        window,
        "Sadboy", // Title maximized
        "Emulator", // Title minimized
        None,
        NULL,
        0,
        NULL
    );

    // set inputs
    XSelectInput(display, window, ExposureMask|KeyPressMask);
    XMapWindow(display, window);
    XFlush(display);

    x11_fd = ConnectionNumber(display);

    // create graphics context
    gc = XCreateGC(display, window, 0, 0);

    // set the fore/back ground colors currently in use in the window
    XSetForeground(display, gc, white);
    XSetBackground(display, gc, white);

    // clear the window and bring to foreground
    XClearWindow(display, window);
    XMapRaised(display, window);

    XDrawString(display, window, gc, 5, 5,"Sadboy world!", strlen("Sadboy world!"));
}

void DisplayManager::close_x() {
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void DisplayManager::init_palette(MMU* mmu) {
    Visual* visual = DefaultVisual(display, screen);

    cmap = XCreateColormap(
        display,
        RootWindow(display, screen),
        visual,
        AllocNone
    );

    for (int ii = 0; ii < 4; ii++) {
        uint8_t mmuColor = mmu->palette[ii];
        Status rc;

        switch (mmuColor) {
            XColor xc;
            case 0:
            {
                rc = XAllocNamedColor(display, cmap, "Gray100", &palette[ii], &xc);
            }
                break;
            case 1:
            {
                rc = XAllocNamedColor(display, cmap, "Gray85", &palette[ii], &xc);
            }
                break;
            case 2:
            {
                rc = XAllocNamedColor(display, cmap, "Gray55", &palette[ii], &xc);
            }
                break;
            case 3:
            {
                rc = XAllocNamedColor(display, cmap, "Gray0", &palette[ii], &xc);
            }
                break;
        }

        if (rc == 0) {
            printf("Color Alloc [%d] failed.\n", ii);
        }
    }

    XSetWindowColormap(display, window, cmap);
}

DisplayManager::DisplayManager(MMU* mmu, uint8_t ws = 1) {
    windowScalar = ws;

    init_x();
    init_palette(mmu);
}

DisplayManager::~DisplayManager() {
    close_x();
}

void DisplayManager::Draw(uint8_t color, uint8_t y, uint8_t x) {
    // printf("color: %d\n", color);
    Draw(palette[color], y, x);
}

void DisplayManager::Draw(XColor color, uint8_t y, uint8_t x) {
    XSetForeground(display, gc, color.pixel);
    XFillRectangle(
        display,
        window,
        gc,
        (x*windowScalar), // x
        (y*windowScalar), // y
        (1*windowScalar), // width
        (1*windowScalar) // height
    );
}

uint16_t DisplayManager::GetKeyInput() {
    FD_ZERO(&in_fds);
    FD_SET(x11_fd, &in_fds);

    tv.tv_usec = 0;
    tv.tv_sec = 0;

    if (select(x11_fd+1, &in_fds, 0, 0, &tv)) {
        // printf("Event Received!\n");

        while (XPending(display)) {
            XNextEvent(display, &event);

            if (event.type == KeyPress) {
                return event.xkey.keycode;
            }
        }
    } else {
        // printf("Timer Fired!\n");
    }

    return 0x00;
}
