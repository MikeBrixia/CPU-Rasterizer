#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

class Rasterizer
{

public:
    
    /**
     * Draw a line segment from pixel p0 to pixel p1
     * @param surface 
     * @param x0 x coordinate of point p0
     * @param y0 y coordinate of point p0
     * @param x1 x coordinate of point p1
     * @param y1 y coordinate of point p1
     * @param mapped_color color of the line.
     */
    void draw_line(SDL_Surface* surface, int x0, int y0, int x1, int y1, uint32_t mapped_color);
    void draw_line(SDL_Surface* surface, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, const SDL_Palette* palette, SDL_Color color);
    void draw_triangle(SDL_Surface* surface, SDL_Palette* palette, SDL_Color color1, SDL_Color color2, SDL_Color color3,
        int x0, int y0, int x1, int y1, int x2, int y2);
    void draw_triangle(SDL_Surface* surface, SDL_Palette* palette, SDL_Surface* texture,
        int x0, int y0, int x1, int y1, int x2, int y2);
    
    static SDL_Color barycentric_interpolation(float alpha, float beta, float gamma,
        SDL_Color color1, SDL_Color color2, SDL_Color color3);
    static SDL_Color texture_interpolation(float u, float v, SDL_Surface* texture, SDL_Palette* palette);

private:
    
    void texture_mapping(float alpha, float beta, float gamma, float& u, float& v);
    void set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, uint32_t value);
    void set_pixel(SDL_Surface* surface, uint32_t x, uint32_t y, const SDL_Palette* palette, SDL_Color color);
};
