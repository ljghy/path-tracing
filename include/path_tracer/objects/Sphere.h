#ifndef SPHERE_H
#define SPHERE_H
#include "path_tracer/Object.h"

class Sphere : public Object
{
protected:
    glm::vec3 m_center;
    float m_radius;

public:
    Sphere(const glm::vec3 &_center, float _radius, Material *_mat = nullptr, bool _invNorm = false)
        : Object(_mat, _invNorm), m_center(_center), m_radius(_radius) {}
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const override
    {
        const glm::vec3 &ori = r.origin;
        const glm::vec3 &dir = r.dir;
        glm::vec3 tmp(m_center - ori);
        float p(glm::dot(dir, tmp)), q(glm::dot(tmp, tmp) - m_radius * m_radius);
        float delta(p * p - q);
        if (delta < 0)
        {
            info.happen = false;
            return;
        }
        delta = glm::sqrt(delta);
        info.t = p - delta;
        if (info.t > 0)
        {
            info.happen = true;
            info.pos = ori + info.t * dir;
            info.normal = (invNorm ? m_center - info.pos : info.pos - m_center) / m_radius;
            info.mat = m_pMat;
            // info.frontFace = glm::dot(r.dir, info.normal) < 0;
            return;
        }
        info.t = p + delta;
        if (info.t > 0)
        {
            info.happen = true;
            info.pos = ori + info.t * dir;
            info.normal = (invNorm ? m_center - info.pos : info.pos - m_center) / m_radius;
            info.mat = m_pMat;
            // info.frontFace = glm::dot(r.dir, info.normal) < 0;
            return;
        }
        info.happen = false;
    }
};
#endif