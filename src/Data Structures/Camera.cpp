
#include "Camera.h"

Camera::Camera(Vector3f eye, float fov, float near_plane, float far_plane, float window_width, float window_height)
{
    this->fov = fov;
    update_camera_matrix(eye, fov);
    projection_matrix = create_perspective_matrix(fov, window_width, window_height, near_plane, far_plane);
}

Camera::Camera(Vector3f eye, float fov, const Matrix4f& projection)
{
    update_camera_matrix(eye, fov);
    projection_matrix = projection;   
}

Camera::Camera(Vector3f eye, float fov, float near_plane, float far_plane, ECameraMode mode, float window, float height)
{
    this->fov = fov;
    update_camera_matrix(eye, fov);

    if (mode == Orthographic)
        projection_matrix = create_orthographic_matrix(fov, window, height, near_plane, far_plane);
    else if (mode == Perspective)
        projection_matrix = create_perspective_matrix(fov, window, height, near_plane, far_plane);
}

void Camera::update_camera_matrix(Vector3f eye, float fov)
{
    Vector3f gaze = Vector3f(0, 0, -1);
    Vector3f view_up = Vector3f(0, 1, 0);

    // Build the camera vector space base to define the camera coordinate system generator.
    Vector3f forward = -gaze;
    Vector3f right = view_up.cross(forward).normalized();
    Vector3f up = forward.cross(right).normalized();
    
    // Vector space base.
    camera_matrix(0, 0) = right.x();
    camera_matrix(0, 1) = right.y();
    camera_matrix(0, 2) = right.z();
    camera_matrix(1, 0) = up.x();
    camera_matrix(1, 1) = up.y();
    camera_matrix(1, 2) = up.z();
    camera_matrix(2, 0) = forward.x();
    camera_matrix(2, 1) = forward.y();
    camera_matrix(2, 2) = forward.z();
    
    // Translation components.
    camera_matrix(0, 3) = -right.dot(eye);
    camera_matrix(1, 3) = -up.dot(eye);
    camera_matrix(2, 3) = -forward.dot(eye);
    camera_matrix(3, 0) = 0.f;
    camera_matrix(3, 1) = 0.f;
    camera_matrix(3, 2) = 0.f;
    camera_matrix(3, 3) = 1.0f;
}

Matrix4f Camera::create_perspective_matrix(float fov, float width, float  height, float near_plane, float far_plane)
{
    Matrix4f orthographic_matrix = create_orthographic_matrix(fov, width, height, near_plane, far_plane);

    Matrix4f perspective_matrix;
    perspective_matrix(0, 0) = near_plane;
    perspective_matrix(1, 1) = near_plane;
    perspective_matrix(2, 2) = far_plane + near_plane;
    perspective_matrix(2, 3) = -(near_plane * far_plane);
    perspective_matrix(3, 2) = 1.f;
    
    return orthographic_matrix * perspective_matrix;
}

Matrix4f Camera::create_orthographic_matrix(float fov, float width, float height, float near_plane, float far_plane)
{
    float fovRad = fov * (float)3.14159265 / 180.f;
    
    float top = near_plane * std::tan(fovRad / 2.f);
    float bottom = -top;
    float right = top * (width/height);
    float left = -right;

    Matrix4f orthographic_matrix;
    // Build the orthographic projection matrix.
    orthographic_matrix(0, 0) = 2/(right - left);
    orthographic_matrix(1, 1) = 2/(top - bottom);
    orthographic_matrix(2, 2) = -2/(far_plane - near_plane);
    orthographic_matrix(3, 3) = 1;
    
    orthographic_matrix(0, 3) = -(right + left)/(right - left);
    orthographic_matrix(1, 3) = -(top + bottom)/(top - bottom);
    orthographic_matrix(2, 3) = -(far_plane + near_plane)/(far_plane - near_plane);

    return orthographic_matrix;   
}

const Matrix4f& Camera::get_camera_matrix() const
{
    return camera_matrix; 
}

const Matrix4f& Camera::get_projection_matrix() const
{
    return projection_matrix;  
}
