#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include "Data Structures/Triangle.h"

class Rasterizer
{

public:
    
    void draw_line(SDL_Surface* surface, SDL_Palette* palette, int x0, int y0, int x1, int y1, SDL_Color color);
    void draw_line(SDL_Surface* surface, int x0, int y0, int x1, int y1, int mapped_color);
    void draw_triangle(SDL_Surface* surface, SDL_Palette* palette, SDL_Surface* texture, const Triangle& triangle);
    static SDL_Color barycentric_interpolation(float alpha, float beta, float gamma,
        SDL_Color color1, SDL_Color color2, SDL_Color color3);

private:

    void map_texture(float alpha, float beta, float gamma,
        float u0, float v0, float u1, float v1, float u2, float v2, float& u, float& v);
    SDL_Color interpolate_texture(float u, float v, SDL_Surface* texture, SDL_Palette* palette);
    void set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, uint32_t value);
    void set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, const SDL_Palette* palette, SDL_Color color);
};
