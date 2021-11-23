#include "path_tracer/Camera.h"
Camera::Camera(const glm::vec3 &_pos, const glm::vec3 &_dir, const glm::vec3 &_up, float _hfov, float _ratio) : camPos(_pos), camDir(glm::normalize(_dir)), camUp(glm::normalize(_up)), hFov(glm::radians(_hfov)), ratio(_ratio)
{
}
Camera::Camera(const glm::vec3 &_pos, const glm::vec3 &_dir, float _hfov, float _ratio) : camPos(_pos), camDir(glm::normalize(_dir)), hFov(glm::radians(_hfov)), ratio(_ratio)
{
    float x(_dir.x), y(_dir.y), z(_dir.z), r(sqrt(x * x + y * y));
    if (ISZERO(r))
    {
        camUp = glm::vec3(1.0, 0.0, 0.0);
    }
    else
    {
        camUp = glm::normalize(glm::vec3(-x * z / r, -y * z / r, r));
    }
}
void Camera::generateRay(Ray &r, float u, float v)
{
    glm::vec3 uAxis(glm::cross(camDir, camUp)), vAxis(-camUp);
    r = Ray(camPos, 0.5f / tanf(0.5 * hFov) * camDir + (u - 0.5f) * uAxis + (v - 0.5f) / ratio * vAxis);
}