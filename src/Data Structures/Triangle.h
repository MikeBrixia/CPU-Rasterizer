#pragma once
#include <SDL3/SDL_pixels.h>

struct Vertex
{
    float x, y, z;
    float u, v;
    SDL_Color color;
};

struct Triangle
{
    Vertex v1, v2, v3;
};
