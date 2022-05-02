#include "path_tracer/objects/Sphere.h"
Sphere::Sphere(const glm::vec3 &_center, float _radius, const std::shared_ptr<Material> &_mat, bool _invNorm)
    : Object(_mat, _invNorm), m_center(_center), m_radius(_radius) {}

void Sphere::rayIntersection(const Ray &r, IntersectionInfo &info) const
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
    delta = sqrt(delta);
    float t(p - delta);
    if (t > 0)
    {
        info.happen = true;
        info.t = t;
        info.pos = ori + t * dir;
        info.normal = glm::normalize(invNorm ? m_center - info.pos : info.pos - m_center);
        info.mat = m_pMat;
        info.frontFace = glm::dot(r.dir, info.normal) < 0;
        return;
    }
    t = p + delta;
    if (t > 0)
    {
        info.happen = true;
        info.t = t;
        info.pos = ori + t * dir;
        info.normal = glm::normalize(invNorm ? m_center - info.pos : info.pos - m_center);
        info.mat = m_pMat;
        info.frontFace = glm::dot(r.dir, info.normal) < 0;
        return;
    }
    info.happen = false;
}