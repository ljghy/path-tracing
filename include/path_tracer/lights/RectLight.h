#ifndef RECT_LIGHT_H
#define RECT_LIGHT_H

#include "path_tracer/Light.h"

class RectLight : public Light
{
protected:
    glm::vec3 m_center;
    glm::vec3 m_normal; // z axis
    glm::vec3 m_xAxis;
    glm::vec3 m_yAxis; // y = cross(z, x)
    float m_a, m_ha;   // x
    float m_b, m_hb;   // y

public:
    RectLight(const glm::vec3 &e, const glm::vec3 &c, const glm::vec3 &n, const glm::vec3 &x, float a, float b)
        : Light(e, a * b),
          m_center(c),
          m_normal(glm::normalize(n)),
          m_xAxis(glm::normalize(x - glm::dot(x, m_normal) * m_normal)),
          m_yAxis(glm::cross(m_normal, m_xAxis)),
          m_a(a), m_b(b), m_ha(0.5f * a), m_hb(0.5f * b) {}

    virtual glm::vec3 sample() const override
    {
        float a = randf(-m_ha, m_ha), b = randf(-m_hb, m_hb);
        return a * m_xAxis + b * m_yAxis + m_center;
    }

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
            info.pos = ori + info.t * dir;
            glm::vec3 p = info.pos - m_center;
            if (fabs(glm::dot(m_xAxis, p)) < m_ha && fabs(glm::dot(m_yAxis, p)) < m_hb)
            {
                info.happen = true;
                info.normal = m_normal;
                info.mat = nullptr;
                // info.frontFace = glm::dot(r.dir, info.normal) < 0;
                info.light = this;
            }
        }
    }
};

#endif