
#pragma once

#include <Eigen>

struct Camera
{
private:
    /**
     * Used to transform world space to camera space.
     */
    Eigen::Matrix4f camera_matrix;

    /**
     * Orthographic projection matrix.
     */
    Eigen::Matrix4f orthographic_matrix;

    /**
     * Camera field of view.
     */
    float fov;

public:
    Camera(Eigen::Vector3<float> eye, float fov);
    void update_camera_matrix(Eigen::Vector3<float> eye, float fov);

    /**
     * Update the orthographic projection matrix used by this camera.
     * @param width The width of the orthographic view volume, usually window width.
     * @param height The height of the orthographic view volume, usually window height.
     * @param near_plane Distance from the camera to the near plane.
     * @param far_plane Distance from the camera to the far plane.
     */
    void update_orthographic_matrix(float width, float  height, float near_plane, float far_plane);
};
