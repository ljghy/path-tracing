#ifndef INFINITE_PLANE_H
#define INFINITE_PLANE_H

#include "path_tracer/Object.h"

class InfinitePlane : public Object
{
protected:
    glm::vec3 m_center;
    glm::vec3 m_normal;

public:
    InfinitePlane(const glm::vec3 &_center, const glm::vec3 &_normal, const std::shared_ptr<Material> &_mat = nullptr, bool _invNorm = false);
    void rayIntersection(const Ray &r, IntersectionInfo &info) const;
};

#endif