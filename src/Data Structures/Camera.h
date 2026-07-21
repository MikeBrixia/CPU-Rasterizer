
#pragma once

#include "../Math/Matrix.h"
#include "../Math/Vector3.h"

enum ECameraMode : uint8_t
{
    Orthographic,
    Perspective,
};

struct Camera
{

private:
    /**
     * Used to transform world space to camera space.
     */
    Matrix4f camera_matrix;

    /**
     * Matrix used to project camera space into canonical view volume.
     */
    Matrix4f projection_matrix;
    
    /**
     * Camera field of view.
     */
    float fov;

public:
    Camera() = default;
    Camera(Vector3f eye, float fov, float near_plane, float far_plane, float window_width, float window_height);
    Camera(Vector3f eye, float fov, const Matrix4f& projection);
    Camera(Vector3f eye, float fov, float near_plane, float far_plane, ECameraMode mode, float window, float height);
    
    void update_camera_matrix(Vector3f eye, float fov);

    /**
     * Create a perspective projection matrix.
     * @param width The width of the perspective view volume, usually window width.
     * @param height The height of the perspective  view volume, usually window height.
     * @param near_plane Distance from the camera to the near plane.
     * @param far_plane Distance from the camera to the far plane.
     */
    static Matrix4f create_perspective_matrix(float fov, float width, float  height, float near_plane, float far_plane);

    /**
     * Create an orthographic projection matrix.
     * @param width The width of the orthographic view volume, usually window width.
     * @param height The height of the orthographic view volume, usually window height.
     * @param near_plane Distance from the camera to the near plane.
     * @param far_plane Distance from the camera to the far plane.
     */
    static Matrix4f create_orthographic_matrix(float fov, float width, float height, float near_plane, float far_plane);
    
    const Matrix4f& get_camera_matrix() const;
    const Matrix4f& get_projection_matrix() const;
};
