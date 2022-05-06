#include "path_tracer/objects/InfinitePlane.h"
InfinitePlane::InfinitePlane(const glm::vec3 &_center, const glm::vec3 &_normal, const std::shared_ptr<Material> &_mat, bool _invNorm)
    : Object(_mat, _invNorm), m_center(_center), m_normal(glm::normalize(_normal)) {}

void InfinitePlane::rayIntersection(const Ray &r, IntersectionInfo &info) const
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