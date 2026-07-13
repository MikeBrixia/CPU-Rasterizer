
#include "Rasterizer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>

#define LINE_EQUATION(x, y, x0, y0, x1, y1) (((y0) - (y1))*(x) + ((x1) - (x0))*(y) + (x0) * (y1) - (x1) * (y0))

void Rasterizer::draw_triangle(SDL_Surface* surface, SDL_Palette* palette, SDL_Surface* texture, const Triangle& triangle)
{
    const Vertex v1 = triangle.v1;
    const Vertex v2 = triangle.v2;
    const Vertex v3 = triangle.v3;
    
    const float alpha_denominator = LINE_EQUATION(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y);
    const float beta_denominator = LINE_EQUATION(v2.x, v2.y, v3.x, v3.y, v1.x, v1.y);
    const float gamma_denominator = LINE_EQUATION(v3.x, v3.y, v1.x, v1.y, v2.x, v2.y);

    const float f12 = LINE_EQUATION(-1, -1, v2.x, v2.y, v3.x, v3.y);
    const float f20 = LINE_EQUATION(-1, -1, v3.x, v3.y, v1.x, v1.y);
    const float f01 = LINE_EQUATION(-1, -1, v1.x, v1.y, v2.x, v2.y);
    
    const int x_min = std::min({v1.x, v2.x, v3.x}); // bottom-left bounding box point x coordinate
    const int x_max = std::max({v1.x, v2.x, v3.x}); // bottom-right bounding box point
    const int y_min = std::min({v1.y, v2.y, v3.y}); // bottom-left bounding box point
    const int y_max = std::max({v1.y, v2.y, v3.y}); // bottom-left bounding box point
    
    for (int x = x_min; x < x_max; ++x)
    {
        for (int y = y_min; y < y_max; ++y)
        {
            float alpha = LINE_EQUATION(x, y, v2.x, v2.y, v3.x, v3.y) / alpha_denominator;
            float beta = LINE_EQUATION(x, y, v3.x, v3.y, v1.x, v1.y) / beta_denominator;
            float gamma = LINE_EQUATION(x, y, v1.x, v1.y, v2.x, v2.y) / gamma_denominator;

            const bool is_inside_triangle = alpha >= 0 && beta >= 0 && gamma >= 0; // Is the point inside the triangle? (barycentric coordinates between 0 and 1)
            // If the pixel lies on a shared edge, the triangle will draw the pixel
            // iff the point opposite to the edge is on the same side of the line
            // as the external point (in our case [-1, 1])
            const bool edge_test = alpha > 0 || alpha_denominator * f12 > 0
                                   && beta > 0 || beta_denominator * f20 > 0
                                   && gamma > 0 || gamma_denominator * f01 > 0;
            if (is_inside_triangle && edge_test)
            {
                SDL_Color color;
                // If this triangle was assigned a texture, map and interpolate it to get the pixel color.
                if (texture != nullptr)
                {
                    float u, v;
                    map_texture(alpha, beta, gamma, v1.u, v1.v, v2.u, v2.v, v3.u, v3.v, u, v);
                    color = interpolate_texture(u, v, texture, palette);
                }
                // Otherwise, interpolate vertex colors with goraud interpolation.
                else
                {
                    color = barycentric_interpolation(alpha, beta, gamma, v1.color, v2.color, v3.color);
                }
                
                uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette,
                    color.r, color.g, color.b, color.a);
                // Set the pixel color.
                set_pixel(surface, x, y, pixel);
            }
        }
    }
}

void Rasterizer::draw_line(SDL_Surface* surface, SDL_Palette* palette, int x0, int y0, int x1, int y1, SDL_Color color)
{
    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(surface->format), palette, color.r, color.g, color.b, color.a);
    draw_line(surface, x0, y0, x1, y1, pixel);
}

void Rasterizer::draw_line(SDL_Surface* surface, int x0, int y0, int x1, int y1, int mapped_color)
{
    float slope = static_cast<float>(std::abs(y1 - y0)) / static_cast<float>(std::abs(x1 - x0));

    // Is the line steep?
    if (slope > 1)
    {
        // In that case, swap X and Y coordinates to fall back in
        // the non-steep case.
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    // Ensures that we always work with rising lines.
    if (y0 > y1)
        std::swap(y0, y1);
    
    int y = y0; // Y starting point.
    
    const int a = y0 - y1; // coefficient of X.
    const int b = x1 - x0; // coefficient of Y.
    
    int eq_result = LINE_EQUATION(x0 + 1, y + 0.5, x0, y0, x1, y1);

    // Iterate over all pixels along the path from start to end.
    for (int x = x0; x < x1; ++x)
    {
        // Fill with color the chosen pixel.
        set_pixel(surface, x, y, mapped_color);
        
        if (eq_result < 0)
        {
            // If true, we need to color the (x+1, y+1) pixel.
            y += 1;
            eq_result += a + b;
        }
        else
        {
            eq_result += a;
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

void Rasterizer::map_texture(float alpha, float beta, float gamma, float u0, float v0, float u1, float v1, float u2,
    float v2, float& u, float& v)
{
    u = alpha * u0 + beta * u1 + gamma * u2;
    v = alpha * v0 + beta * v1 + gamma * v2;
}

SDL_Color Rasterizer::interpolate_texture(float u, float v, SDL_Surface* texture, SDL_Palette* palette)
{
    float w = static_cast<float>(texture->w - 1);
    float h = static_cast<float>(texture->h - 1);

    // Compute pixel coordinates represented by uv coordinates.
    float x_pixel = u * w;
    float y_pixel = v * h;

    // Keep pixel coordinates inside bounds.
    x_pixel = std::max(0.0f, std::min(x_pixel, w));
    y_pixel = std::max(0.0f, std::min(y_pixel, h));

    // Truncate x, y pixels to nearest pixel coordinates value.
    const int tex_x = static_cast<int>(x_pixel);
    const int tex_y = static_cast<int>(y_pixel);

    // Find the pixel color at the computed pixel coordinates.
    uint32_t* texture_pixels = static_cast<uint32_t*>(texture->pixels);
    uint32_t pixel = texture_pixels[tex_y * (texture->pitch / sizeof(uint32_t)) + tex_x];
    
    SDL_Color color;
    const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(texture->format);
    SDL_GetRGBA(pixel, details, palette, &color.r, &color.g, &color.b, &color.a);
    
    return color;
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

