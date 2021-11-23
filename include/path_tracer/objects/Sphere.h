#ifndef SPHERE_H
#define SPHERE_H
#include "path_tracer/Object.h"
class Sphere : public Object
{
protected:
    glm::vec3 m_center;
    float m_radius;

public:
    Sphere(const glm::vec3 &_center, float _radius, Material *_mat = nullptr);
    void rayIntersection(const Ray &r, IntersectionInfo &info);
};
#endif