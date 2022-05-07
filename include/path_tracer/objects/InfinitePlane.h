#ifndef INFINITE_PLANE_H
#define INFINITE_PLANE_H

#include "path_tracer/Object.h"

class InfinitePlane : public Object
{
protected:
    glm::vec3 m_center;
    glm::vec3 m_normal;

public:
    InfinitePlane(const glm::vec3 &_center, const glm::vec3 &_normal, Material *_mat = nullptr, bool _invNorm = false)
        : Object(_mat, _invNorm), m_center(_center), m_normal(glm::normalize(_normal)) {}
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const override
    {
        const glm::vec3 &ori = r.origin;
        const glm::vec3 &dir = r.dir;

        info.happen = false;
        float a = glm::dot(dir, m_normal);
        if (ISZERO(a))
            return;
        info.t = glm::dot(m_center - ori, m_normal) / a;

        if (info.t > 0)
        {
            info.happen = true;
            info.pos = ori + info.t * dir;
            info.normal = invNorm ? -m_normal : m_normal;
            info.mat = m_pMat;
            // info.frontFace = glm::dot(r.dir, info.normal) < 0;
            return;
        }
    }
};

#endif