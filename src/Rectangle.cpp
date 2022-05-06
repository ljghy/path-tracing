#include "path_tracer/objects/Rectangle.h"

Rectangle::Rectangle(const glm::vec3 &c, const glm::vec3 &n, const glm::vec3 &x, float a, float b, const std::shared_ptr<Material> &_mat, bool _invNorm)
    : Object(_mat, _invNorm),
      m_center(c),
      m_normal(glm::normalize(_invNorm ? -n : n)),
      m_xAxis(glm::normalize(x - glm::dot(x, m_normal) * m_normal)),
      m_yAxis(glm::cross(m_normal, m_xAxis)),
      m_a(a), m_b(b), m_ha(0.5f * a), m_hb(0.5f * b) {}

void Rectangle::rayIntersection(const Ray &r, IntersectionInfo &info) const
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
        info.pos = ori + info.t * dir;
        glm::vec3 p = info.pos - m_center;
        if (glm::abs(glm::dot(m_xAxis, p)) < m_ha && glm::abs(glm::dot(m_yAxis, p)) < m_hb)
        {
            info.happen = true;
            info.normal = m_normal;
            info.mat = m_pMat;
            // info.frontFace = glm::dot(r.dir, info.normal) < 0;
        }
    }
}