#pragma once

#include <SDL3/SDL_video.h>

class Application
{
    
private:
    uint16_t WINDOW_WIDTH = 800;
    uint16_t WINDOW_HEIGHT = 600;
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Palette* palette;
    
public:
    Application();
    ~Application();
    
    void run();
    void init_window();
    void init_surface();
    void loop();
    void rasterize();
    void render();
    void terminate();
};
