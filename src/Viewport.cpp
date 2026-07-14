
#include "Viewport.h"
#include <iostream>
#include <SDL3/SDL_events.h>

Viewport::Viewport(const std::string& name, uint16_t width, uint16_t height): surface(nullptr), palette(nullptr)
{
    this->height = height;
    this->width = width;

    window = SDL_CreateWindow(name.c_str(), width, height, SDL_WINDOWPOS_UNDEFINED);
}

Viewport::~Viewport()
{
    SDL_DestroyWindow(window);
    SDL_DestroyPalette(palette);
    SDL_DestroySurface(surface);
    delete rasterizer;
}

void Viewport::init()
{
    surface = SDL_GetWindowSurface(window);
    palette = SDL_CreatePalette(256);

    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

    // Print any SDL error to the console.
    std::cout << SDL_GetError() << '\n';

    SDL_Color color{30, 144, 255, 255};
    SDL_SetSurfacePalette(surface, palette);
    const uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format),
                                       palette, color.r, color.g, color.b, color.a);
    SDL_FillSurfaceRect(surface, nullptr, pixel);

    SDL_UpdateWindowSurface(window);
    
    viewport_matrix.setIdentity();
    // Initialize viewport matrix diagonal.
    viewport_matrix[0][0] = width / 2;
    viewport_matrix[1][1] = height / 2;

    // Initialize translation components.
    viewport_matrix[0][3] = (width - 1) / 2;
    viewport_matrix[1][3] = (height - 1) / 2;
    viewport_matrix[2][3] = 0;

    this->state = EViewportState::Running;
}

void Viewport::update()
{
    for (SDL_Event event; SDL_PollEvent(&event);)
    {
        switch (event.type)
        {
        default: break;
                
        case SDL_EVENT_WINDOW_RESIZED:
            {
                // Set a new window size.
                width = static_cast<uint8_t>(event.window.data1);
                height = static_cast<uint8_t>(event.window.data2);
                    
                // Init new viewport with updates properties.
                init();
                break;
            }
                
        case SDL_EVENT_QUIT:
            state = EViewportState::Stopped;
            break;
                
        case SDL_EVENT_MOUSE_MOTION:
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            break;
        }
    }
}

void Viewport::render()
{
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
            rasterizer->draw_triangle(surface, palette, optimized_image, triangle);
            rasterizer->draw_line(surface, palette, 400, 200, 650, 220, SDL_Color {255, 0, 0, 255});
            rasterizer->draw_line(surface, palette, 200, 200, 250, 300, SDL_Color {255, 0, 0, 255});
            
            SDL_UnlockSurface(surface);
            SDL_DestroySurface(optimized_image);
        }
    }
    
    SDL_UpdateWindowSurface(window);
}

EViewportState Viewport::get_state() const
{
    return state;
}
