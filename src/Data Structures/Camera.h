#pragma once

#include "glm.hpp"

struct Camera
{
    glm::mat4 model;
    float fov;
    
    Camera(glm::vec3 eye, float fov);
};
