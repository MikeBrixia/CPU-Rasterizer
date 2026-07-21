#include "Mesh.h"

Mesh Mesh::create_cube(float size, SDL_Color color, SDL_Surface* texture)
{
    Mesh cube;

    cube.texture = texture;
    // Half-dimension of the cube.
    float half = size / 2;
    cube.vertices.reserve(8);
    cube.vertices.emplace_back(-half, -half, -half,  0.0f, 0.0f,  SDL_Color{255, 0, 0, 255});
    cube.vertices.emplace_back( half, -half, -half,  1.0f, 0.0f,  SDL_Color{0, 255, 0, 255}); 
    cube.vertices.emplace_back( half,  half, -half,  1.0f, 1.0f,  SDL_Color{0, 0, 255, 255}); 
    cube.vertices.emplace_back(-half,  half, -half,  0.0f, 1.0f,  SDL_Color{255, 0, 0, 255});
    cube.vertices.emplace_back(-half, -half,  half,  0.0f, 0.0f,  SDL_Color{0, 255, 0, 255}); 
    cube.vertices.emplace_back( half, -half,  half,  1.0f, 0.0f,  SDL_Color{0, 0, 255, 255}); 
    cube.vertices.emplace_back( half,  half,  half,  1.0f, 1.0f,  SDL_Color{255, 0, 0, 255}); 
    cube.vertices.emplace_back(-half,  half,  half,  0.0f, 1.0f,  SDL_Color{0, 255, 0, 255});
    
    cube.indices = {
        // (Z-)
        1, 0, 3,
        3, 2, 1,

        // (Z+)
        4, 5, 6,
        6, 7, 4,

        // (X-)
        0, 4, 7,
        7, 3, 0,

        // (X+)
        5, 1, 2,
        2, 6, 5,

        // (Y+)
        3, 2, 6,
        6, 7, 3,

        // (Y-)
        1, 5, 4,
        4, 0, 1
    };

    
    return cube;
}
