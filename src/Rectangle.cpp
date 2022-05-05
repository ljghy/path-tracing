#include "path_tracer/objects/Rectangle.h"

Rectangle::Rectangle(const glm::vec3 &c, const glm::vec3 &n, const glm::vec3 &x, float a, float b, const std::shared_ptr<Material> &_mat, bool _invNorm)
    : Object(_mat, _invNorm),
      m_center(c),
      m_normal(glm::normalize(n)),
      m_xAxis(glm::normalize(x - glm::dot(x, m_normal) * m_normal)),
      m_yAxis(glm::cross(m_normal, m_xAxis)),
      m_a(a), m_b(b) {}

void Rectangle::rayIntersection(const Ray &r, IntersectionInfo &info) const
{
    const glm::vec3 &ori = r.origin;
    const glm::vec3 &dir = r.dir;

    info.happen = false;
    float a = glm::dot(dir, m_normal);
    if (ISZERO(a))
        return;
    float t = glm::dot(m_center - ori, m_normal) / a;

    if (t > 0)
    {
        info.pos = ori + t * dir;
        glm::vec3 p = info.pos - m_center;
        float a = glm::dot(m_xAxis, p), b = glm::dot(m_yAxis, p);
        if (fabs(a) < 0.5f * m_a && fabs(b) < 0.5f * m_b)
        {
            info.happen = true;
            info.t = t;
            info.pos = ori + t * dir;
            info.normal = glm::normalize(invNorm ? -m_normal : m_normal);
            info.mat = m_pMat;
            info.frontFace = glm::dot(r.dir, info.normal) < 0;
        }
    }
}