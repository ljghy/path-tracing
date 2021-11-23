#ifndef CAMERA_H
#define CAMERA_H
#include "path_tracer/Math.h"
#include "path_tracer/Ray.h"
struct Camera
{
    glm::vec3 camPos;
    glm::vec3 camDir;
    glm::vec3 camUp;
    float hFov;  // rad
    float ratio; // width:height

    Camera(const glm::vec3 &_pos, const glm::vec3 &_dir, const glm::vec3 &_up, float _hfov, float _ratio); //_hfov(degree)
    Camera(const glm::vec3 &_pos, const glm::vec3 &_dir, float _hfov, float _ratio);                       //default up vector

    void generateRay(Ray &r, float u, float v); //u,v from 0.0 to 1.0
};
#endif