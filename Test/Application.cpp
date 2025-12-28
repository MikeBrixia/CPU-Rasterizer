
#include "Application.h"
#include <algorithm>
#include <complex>
#include <iostream>
#include <__msvc_ostream.hpp>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include "../src/Rasterizer.h"

Application::Application(): window(nullptr)
{
}

Application::~Application()
{
}

void Application::run()
{
    init_window();
    loop();
    terminate();
}

void Application::init_window()
{
    window = SDL_CreateWindow("CPU Raster",
    WINDOW_WIDTH, WINDOW_HEIGHT,
    SDL_WINDOW_RESIZABLE);

    init_surface();
}

void Application::init_surface()
{
    surface = SDL_GetWindowSurface(window);
    palette = SDL_CreatePalette(256);
    
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    
    std::cout << SDL_GetError() << std::endl;
    
    SDL_Color color{30, 144, 255, 255};
    SDL_SetSurfacePalette(surface, palette);
    const uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format),
        palette, color.r, color.g, color.b, color.a);
    SDL_FillSurfaceRect(surface, nullptr, pixel);
    
    SDL_UpdateWindowSurface(window);
}

void Application::loop()
{
    int mouse_x = 0;
    int mouse_y = 0;
    
    bool running = true;
    while (running)
    {
        for (SDL_Event event; SDL_PollEvent(&event);)
        {
            switch (event.type)
            {
            default: break;
                
            case SDL_EVENT_WINDOW_RESIZED:
                {
                    // Set a new window size.
                    WINDOW_WIDTH = static_cast<uint8_t>(event.window.data1);
                    WINDOW_HEIGHT = static_cast<uint8_t>(event.window.data2);
                    
                    // Update the surface.
                    init_surface();
                    break;
                }
                
            case SDL_EVENT_QUIT:
                running = false;
                break;
                
            case SDL_EVENT_MOUSE_MOTION:
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                break;
            }
        }
        render();
    }
}

void Application::rasterize()
{
    Rasterizer rasterizer;
    
    SDL_Surface* texture = SDL_LoadBMP("resources/Testimone.bmp");
    
    std::cout << SDL_GetError() << std::endl;

    if (texture) {
        // Convert texture surface to destination surface format.
        SDL_Surface* optimized_image = SDL_ConvertSurface(texture, surface->format);

        if (optimized_image) {
            rasterizer.draw_triangle(surface, palette, optimized_image, 100, 100, 400, 100, 250, 400);
            //rasterizer.draw_triangle(surface, palette, image, 100, 100, 400, 100, 250, 400);
            SDL_DestroySurface(optimized_image);
        }
    }
    
    SDL_UpdateWindowSurface(window);
}

void Application::render()
{
    rasterize();
}

void Application::terminate()
{
}
