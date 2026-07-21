#pragma once

#include <vector>
#include "Camera.h"
#include "Mesh.h"

struct Scene
{
    Camera camera;
    std::vector<Mesh> meshes;
    
    Scene(const Camera& camera, const std::vector<Mesh>& meshes);
};
