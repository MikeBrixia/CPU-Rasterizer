
#include "Application.h"
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
    init_viewport();
    
    surface = SDL_GetWindowSurface(window);
    palette = SDL_CreatePalette(256);
    
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    
    std::cout << SDL_GetError() << std::endl;
    
    SDL_Color color {30, 144, 255, 255};
    SDL_SetSurfacePalette(surface, palette);
    const uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format),
        palette, color.r, color.g, color.b, color.a);
    SDL_FillSurfaceRect(surface, nullptr, pixel);
    
    SDL_UpdateWindowSurface(window);
}

void Application::init_viewport()
{
    // Initialize viewport matrix diagonal.
    viewport_matrix[0][0] = WINDOW_WIDTH / 2;
    viewport_matrix[1][1] = WINDOW_HEIGHT / 2;
    viewport_matrix[2][2] = 1;
    viewport_matrix[3][3] = 1;

    // Initialize translation components.
    viewport_matrix[0][3] = (WINDOW_WIDTH - 1) / 2;
    viewport_matrix[1][3] = (WINDOW_HEIGHT - 1) / 2;
    viewport_matrix[2][3] = 0;
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
    
    std::cout << SDL_GetError() << '\n';

    if (texture) {
        // Convert texture surface to destination surface format.
        SDL_Surface* optimized_image = SDL_ConvertSurface(texture, surface->format);

        if (optimized_image) {
            SDL_LockSurface(surface);

            Triangle triangle;
            triangle.v1 = Vertex {0, 0, 0, 0, 0, SDL_Color {255, 0, 0, 255}};
            triangle.v2 = Vertex {200, 0, 0, 1, 0, SDL_Color {0, 255, 0, 255}};
            triangle.v3 = Vertex {100, 200, 0, 0.5, 1, SDL_Color {0, 0, 255, 255}};
            rasterizer.draw_triangle(surface, palette, optimized_image, triangle);
            rasterizer.draw_line(surface, palette, 400, 200, 650, 220, SDL_Color {255, 0, 0, 255});
            rasterizer.draw_line(surface, palette, 200, 200, 250, 300, SDL_Color {255, 0, 0, 255});
            
            SDL_UnlockSurface(surface);
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
