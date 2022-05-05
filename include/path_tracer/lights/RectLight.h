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
    float m_a;         // x
    float m_b;         // y

public:
    RectLight(const glm::vec3 &e, const glm::vec3 &c, const glm::vec3 &n, const glm::vec3 &x, float a, float b)
        : Light(e, a * b),
          m_center(c),
          m_normal(glm::normalize(n)),
          m_xAxis(glm::normalize(x - glm::dot(x, m_normal) * m_normal)),
          m_yAxis(glm::cross(m_normal, m_xAxis)),
          m_a(a), m_b(b) {}

    virtual glm::vec3 sample() const override
    {
        float a = 0.5f * randf(-m_a, m_a), b = 0.5f * randf(-m_b, m_b);
        return a * m_xAxis + b * m_yAxis + m_center;
    }

    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const
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
                info.normal = m_normal;
                info.mat = nullptr;
                info.frontFace = glm::dot(r.dir, info.normal) < 0;
                info.id = id;
            }
        }
    }
};

#endif