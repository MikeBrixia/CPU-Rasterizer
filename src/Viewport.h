
#pragma once

#include <string>
#include <SDL3/SDL_video.h>
#include "Rasterizer.h"
#include "Data Structures/Scene.h"
#include "Math/Matrix.h"

enum EViewportState : uint8_t
{
    Running,
    Paused,
    Stopped,   
};

class Viewport
{
    uint16_t width ;
    uint16_t height;
    EViewportState state;

    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Palette* palette;
    Rasterizer* rasterizer;
    
    // Mouse coordinates inside the viewport.
    float mouse_x, mouse_y;
    
    /**
     * Transforms canonical-view-volume into screen space.
     */
    Matrix4f viewport_matrix;

public:
    Scene scene;
    
    Viewport(const std::string& name, uint16_t width, uint16_t height);
    ~Viewport();
    
    void init();
    void update();
    void render(const Scene& scene);
    EViewportState get_state() const;
};
