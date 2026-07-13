
#include "Camera.h"

Camera::Camera(glm::vec3 eye, float fov)
{
    glm::vec3 gaze = glm::normalize(eye);
    glm::vec3 view_up = glm::vec3(0, 1, 0);
    
    glm::vec3 forward = glm::normalize(-eye);
    glm::vec3 right = glm::normalize(glm::cross(view_up, gaze));
    glm::vec3 up = glm::normalize(glm::cross(forward, right));
    
    glm::mat4 camera_matrix = glm::mat4(1.0f);
    camera_matrix[0][0] = right.x;
    camera_matrix[0][1] = right.y;
    camera_matrix[0][2] = right.z;
    camera_matrix[0][3] = -glm::dot(right, eye);
    camera_matrix[1][0] = up.x;
    camera_matrix[1][1] = up.y;
    camera_matrix[1][2] = up.z;
    camera_matrix[1][3] = -glm::dot(up, eye);
    camera_matrix[2][0] = forward.x;
    camera_matrix[2][1] = forward.y;
    camera_matrix[2][2] = forward.z;
    camera_matrix[2][3] = -glm::dot(forward, eye);
    camera_matrix[3][0] = 0.f;
    camera_matrix[3][1] = 0.f;
    camera_matrix[3][2] = 0.f;
    camera_matrix[3][3] = 1.0f;
    
    model = camera_matrix;
}
