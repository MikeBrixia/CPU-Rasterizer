
#pragma once

#include <vector>
#include <SDL3/SDL_render.h>
#include "Vertex.h"

struct Mesh
{
    Matrix4f model;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    SDL_Surface* texture;

    static Mesh create_cube(float size, SDL_Color color = { 255, 255, 255, 255 }, SDL_Surface* texture = nullptr);
};
