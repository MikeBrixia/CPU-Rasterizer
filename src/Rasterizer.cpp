
#include "Rasterizer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
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
    const float alpha_denominator = LINE_EQUATION(x0, y0, x1, y1, x2, y2);
    const float beta_denominator = LINE_EQUATION(x1, y1, x2, y2, x0, y0);
    const float gamma_denominator = LINE_EQUATION(x2, y2, x0, y0, x1, y1);

    const float f12 = LINE_EQUATION(-1, -1, x1, y1, x2, y2);
    const float f20 = LINE_EQUATION(-1, -1, x2, y2, x0, y0);
    const float f01 = LINE_EQUATION(-1, -1, x0, y0, x1, y1);
    
    const int x_min = std::min({x0, x1, x2}); // bottom-left bounding box point x coordinate
    const int x_max = std::max({x0, x1, x2}); // bottom-right bounding box point
    const int y_min = std::min({y0, y1, y2}); // bottom-left bounding box point
    const int y_max = std::max({y0, y1, y2}); // bottom-left bounding box point
    
    for (int x = x_min; x < x_max; ++x)
    {
        for (int y = y_min; y < y_max; ++y)
        {
            float alpha = LINE_EQUATION(x, y, x1, y1, x2, y2) / alpha_denominator;
            float beta = LINE_EQUATION(x, y, x2, y2, x0, y0) / beta_denominator;
            float gamma = LINE_EQUATION(x, y, x0, y0, x1, y1) / gamma_denominator;

            const bool is_inside_triangle = alpha >= 0 && beta >= 0 && gamma >= 0; // Is the point inside the triangle? (barycentric coordinates between 0 and 1)
            // If the pixel lies on a shared edge, the triangle will draw the pixel
            // iff the point opposite to the edge is on the same side of the line
            // as the external point (in our case [-1, 1])
            const bool edge_test = alpha > 0 || alpha_denominator * f12 > 0
                                   && beta > 0 || beta_denominator * f20 > 0
                                   && gamma > 0 || gamma_denominator * f01 > 0;
            if (is_inside_triangle && edge_test)
            {
                SDL_Color interpolated_color = barycentric_interpolation(alpha, beta, gamma, color1, color2, color3);
                uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette,
                    interpolated_color.r, interpolated_color.g, interpolated_color.b, interpolated_color.a);
                // Set the pixel color.
                set_pixel(surface, x, y, pixel);
            }
        }
    }
}

void Rasterizer::draw_triangle(SDL_Surface* surface, SDL_Palette* palette, SDL_Surface* texture, int x0, int y0, int x1,
    int y1, int x2, int y2)
{
    const float alpha_denominator = LINE_EQUATION(x0, y0, x1, y1, x2, y2);
    const float beta_denominator = LINE_EQUATION(x1, y1, x2, y2, x0, y0);
    const float gamma_denominator = LINE_EQUATION(x2, y2, x0, y0, x1, y1);

    const float f12 = LINE_EQUATION(-1, -1, x1, y1, x2, y2);
    const float f20 = LINE_EQUATION(-1, -1, x2, y2, x0, y0);
    const float f01 = LINE_EQUATION(-1, -1, x0, y0, x1, y1);
    
    const int x_min = std::min({x0, x1, x2}); // bottom-left bounding box point x coordinate
    const int x_max = std::max({x0, x1, x2}); // bottom-right bounding box point
    const int y_min = std::min({y0, y1, y2}); // bottom-left bounding box point
    const int y_max = std::max({y0, y1, y2}); // bottom-left bounding box point
    
    for (int x = x_min; x < x_max; ++x)
    {
        for (int y = y_min; y < y_max; ++y)
        {
            float alpha = LINE_EQUATION(x, y, x1, y1, x2, y2) / alpha_denominator;
            float beta = LINE_EQUATION(x, y, x2, y2, x0, y0) / beta_denominator;
            float gamma = LINE_EQUATION(x, y, x0, y0, x1, y1) / gamma_denominator;

            const bool is_inside_triangle = alpha >= 0 && beta >= 0 && gamma >= 0; // Is the point inside the triangle? (barycentric coordinates between 0 and 1)
            // If the pixel lies on a shared edge, the triangle will draw the pixel
            // iff the point opposite to the edge is on the same side of the line
            // as the external point (in our case [-1, 1])
            const bool edge_test = alpha > 0 || alpha_denominator * f12 > 0
                                   && beta > 0 || beta_denominator * f20 > 0
                                   && gamma > 0 || gamma_denominator * f01 > 0;
            if (is_inside_triangle && edge_test)
            {
                float u, v;
                texture_mapping(alpha, beta, gamma, u, v);
                SDL_Color interpolated_color = texture_interpolation(u, v, texture, palette);
                uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette,
                    interpolated_color.r, interpolated_color.g, interpolated_color.b, interpolated_color.a);
                // Set the pixel color.
                set_pixel(surface, x, y, pixel);
            }
        }
    }
}

SDL_Color Rasterizer::barycentric_interpolation(float alpha, float beta, float gamma, SDL_Color color1, SDL_Color color2,
                                                SDL_Color color3)
{
    float r = alpha * static_cast<float>(color1.r) + beta * static_cast<float>(color2.r) + gamma * static_cast<float>(color3.r);
    float g = alpha * static_cast<float>(color1.g) + beta * static_cast<float>(color2.g) + gamma * static_cast<float>(color3.g);
    float b = alpha * static_cast<float>(color1.b) + beta * static_cast<float>(color2.b) + gamma * static_cast<float>(color3.b);
    // result of the Gouraud interpolation.
    return SDL_Color{static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), 255};
}

SDL_Color Rasterizer::texture_interpolation(float u, float v, SDL_Surface* texture, SDL_Palette* palette)
{
    float w = static_cast<float>(texture->w - 1);
    float h = static_cast<float>(texture->h - 1);
    
    u = u * w;
    v = v * h;

    u = std::max(0.0f, std::min(u, w));
    v = std::max(0.0f, std::min(v, h));

    int texX = static_cast<int>(u);
    int texY = static_cast<int>(v);
    
    uint32_t* texture_pixels = static_cast<uint32_t*>(texture->pixels);
    uint32_t pixel = texture_pixels[texY * (texture->pitch / sizeof(uint32_t)) + texX];
    
    SDL_Color color;
    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(texture->format);
    SDL_GetRGBA(pixel, details, palette, &color.r, &color.g, &color.b, &color.a);
    
    return color;
}

void Rasterizer::texture_mapping(float alpha, float beta, float gamma, float& u, float& v)
{
    float u0 = 0.0f, v0 = 0.0f;
    float u1 = 1.0f, v1 = 0.0f;
    float u2 = 0.5f, v2 = 1.0f;

    u = alpha * u0 + beta * u1 + gamma * u2;
    v = alpha * v0 + beta * v1 + gamma * v2;
}

void Rasterizer::set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, uint32_t value)
{
    uint8_t r, g, b, a;
    SDL_GetRGBA(value, SDL_GetPixelFormatDetails(surface->format), nullptr, &r, &g, &b, &a);
    SDL_WriteSurfacePixel(surface, x, y, r, g, b, a);
}

void Rasterizer::set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, const SDL_Palette* palette, SDL_Color color)
{
    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette, color.r, color.g, color.b, color.a);
    set_pixel(surface, x, y, pixel);
}

#undef LINE_EQUATION

