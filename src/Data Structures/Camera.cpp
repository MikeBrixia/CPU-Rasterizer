
#include "Camera.h"

Camera::Camera(Eigen::Vector3<float> eye, float fov)
{
    update_camera_matrix(eye, fov);
}

void Camera::update_camera_matrix(Eigen::Vector3<float> eye, float fov)
{
    Eigen::Vector3<float> gaze = eye.normalized();
    Eigen::Vector3<float> view_up = Eigen::Vector3<float>(0, 1, 0);

    // Build the camera vector base to define the camera coordinate system generator.
    Eigen::Vector3<float> forward = -gaze;
    Eigen::Vector3<float> right = view_up.cross(forward).normalized();
    Eigen::Vector3<float> up = forward.cross(right).normalized();

    // Build the world-to-camera matrix transformation.
    camera_matrix(0, 0) = right.x;
    camera_matrix(0, 1) = right.y;
    camera_matrix(0, 2) = right.z;
    camera_matrix(0, 3) = -right.dot(eye);
    camera_matrix(1, 0) = up.x;
    camera_matrix(1, 1) = up.y;
    camera_matrix(1, 2) = up.z;
    camera_matrix(1, 3) = -up.dot(eye);
    camera_matrix(2, 0) = forward.x;
    camera_matrix(2, 1) = forward.y;
    camera_matrix(2, 2) = forward.z;
    camera_matrix(2, 3) = -forward.dot(eye);
    camera_matrix(3, 0) = 0.f;
    camera_matrix(3, 1) = 0.f;
    camera_matrix(3, 2) = 0.f;
    camera_matrix(3, 3) = 1.0f;
}

void Camera::update_orthographic_matrix(float width, float  height, float near_plane, float far_plane)
{
    float left = -width / 2.f;
    float right = width / 2.f;
    float bottom = -height / 2.f;
    float top = height / 2.f;
    
    orthographic_matrix.setIdentity();
    orthographic_matrix(0, 0) = 2/(right - left);
    orthographic_matrix(1, 1) = 2/(top - bottom);
    orthographic_matrix(2, 2) = -2/(far_plane - near_plane);
    orthographic_matrix(3, 0) = -(right + left)/(right - left);
    orthographic_matrix(3, 1) = -(top + bottom)/(top - bottom);
    orthographic_matrix(3, 2) = -(far_plane + near_plane)/(far_plane - near_plane);
}
