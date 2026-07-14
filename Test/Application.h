#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include "../src/Viewport.h"

class Application
{
    
private:
    uint16_t WINDOW_WIDTH = 800;
    uint16_t WINDOW_HEIGHT = 600;
    Viewport* viewport;

    // Windowing transformation from canonic-view-volume to screen-space.
    int viewport_matrix[4][4] = {};
    
public:
    Application();
    ~Application();

    void init();
    void run();
};
