
#pragma once

#include <SDL3/SDL_pixels.h>
#include "../Math/Vector3.h"

struct Vertex
{
    float x, y, z;
    float u, v;
    SDL_Color color;

    Vertex(float x, float y, float z, float u, float v, SDL_Color color) : x(x), y(y), z(z), u(u), v(v), color(color) {}
};
