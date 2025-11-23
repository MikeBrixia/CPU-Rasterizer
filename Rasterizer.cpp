
#include "Rasterizer.h"
#include <cmath>
#include <utility>

#define LINE_EQUATION(x, y, x0, y0, x1, y1) (((y0) - (y1))*(x) + ((x1) - (x0))*(y) + (x0) * (y1) - (x1) * y0)

void Rasterizer::draw_line(SDL_Surface* surface, int x0, int y0, int x1, int y1, uint32_t mapped_color)
{
    SDL_LockSurface(surface);

    // An absolute slope that is greater than 1 is considered steep.
    const bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
    
    // For steep lines, we need to exchange X coords with Y coords.
    // Basically we'll move forward by Y pixels and move up or down by X,
    // the opposite of non-steep lines.
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);       
    }
    
    // enforce x0 <= x1
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    int y = y0; // Y starting point.
    
    const int a = y0 - y1; // coefficient of X.
    const int b = x1 - x0; // Y coefficient of Y and delta X.
    const int c = x0 * y1 - x1 * y0; // offset.
    
    int eq_result = LINE_EQUATION(x0 + 1, y + 0.5, x0, y0, x1, y1);

    const int dy = y1 - y0; // delta Y.
    const int y_step = dy >= 0 ? 1 : -1; // Delta Y > 0 implies positive growth, negative otherwise. Determines a Y movement direction.

    for (int x = x0; x < x1; ++x)
    {
        if (steep)
        {
            set_pixel(surface, y, x, mapped_color);
        }
        else
        {
            set_pixel(surface, x, y, mapped_color);       
        }
        
        // check if current pixel coordinates are below the line
        if (eq_result < 0)
        {
            // If true, also increment the Y coordinate by one unit.
            // Going up or down by 1 row depends on the y_step.
            y += y_step;
            eq_result += a + b;
        }
        else
        {
            // check if current pixel coordinates are below the line
            if (eq_result < 0)
            {
                // If true, also increment the Y coordinate by one unit.
                // Going up or down by 1 row depends on the y_step.
                y += y_step;
                eq_result += a + b;
            }
            else
            {
                // otherwise we just keep going on the same row of pixels
                // and compute: a(x+1) + by + c.
                eq_result += a * y_step;
            }
        }
    }

    SDL_UnlockSurface(surface);
}

void Rasterizer::draw_line(SDL_Surface* surface, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
    const SDL_Palette* palette, SDL_Color color)
{
    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette, color.r, color.g, color.b, color.a);
    draw_line(surface, x0, y0, x1, y1, pixel);   
}

void Rasterizer::draw_triangle(SDL_Surface* surface, SDL_Palette* palette, SDL_Color color1, SDL_Color color2, SDL_Color color3,
        int x0, int y0, int x1, int y1, int x2, int y2)
{
    float alpha_denominator = LINE_EQUATION(x0, y0, x1, y1, x2, y2);
    float beta_denominator = LINE_EQUATION(x1, y1, x2, y2, x0, y0);
    float gamma_denominator = LINE_EQUATION(x2, y2, x0, y0, x1, y1);

    // create a bounding box around triangle vertices.
    int x_min = std::min(x0, std::min(x1, x2));
    int x_max = std::max(x0, std::max(x1, x2));
    int y_min = std::min(y0, std::min(y1, y2));
    int y_max = std::max(y0, std::max(y1, y2));
    
    for (int x = x_min; x < x_max; ++x)
    {
        for (int y = y_min; y < y_max; ++y)
        {
            float alpha = LINE_EQUATION(x, y, x1, y1, x2, y2) / alpha_denominator;
            float beta = LINE_EQUATION(x, y, x2, y2, x0, y0) / beta_denominator;
            float gamma = LINE_EQUATION(x, y, x0, y0, x1, y1) / gamma_denominator;
            
            // Is the point inside the triangle? (barycentric coordinates between 0 and 1)
            if (alpha > 0 && beta > 0 && gamma > 0)
            {
                float r = alpha * color1.r + beta * color2.r + gamma * color3.r;
                float g = alpha * color1.g + beta * color2.g + gamma * color3.g;
                float b = alpha * color1.b + beta * color2.b + gamma * color3.b;
                
                // result of the Gouraud interpolation.
                SDL_Color interpolated_color = SDL_Color{ static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), 255};
                uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette,
                    interpolated_color.r, interpolated_color.g, interpolated_color.b, interpolated_color.a);
                // Set the pixel color.
                set_pixel(surface, x, y, pixel);
            }
        }
    }
}

void Rasterizer::set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, uint32_t value)
{
    uint32_t* pixels = static_cast<uint32_t*>(surface->pixels);
    // The offset in memory of the pixel we want to set. we're going to offset down Y times by the pitch for the rows
    // and then move forward by X amount on columns.
    int offset = (surface->pitch / sizeof(uint32_t)) * y + x;
    // Find the pixel value stored at 'pixel + offset' memory coordinates and set it's mapped color value.
    *(pixels + offset) = value;
}

void Rasterizer::set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, const SDL_Palette* palette, SDL_Color color)
{
    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette, color.r, color.g, color.b, color.a);
    set_pixel(surface, x, y, pixel);
}

#undef LINE_EQUATION

