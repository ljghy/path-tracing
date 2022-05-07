#ifndef DISK_LIGHT_H
#define DISK_LIGHT_H

#include "path_tracer/Light.h"

class DiskLight : public Light
{
protected:
    glm::vec3 m_center;
    glm::vec3 m_normal;
    float m_radius;
    glm::mat3x3 h;

public:
    DiskLight(const glm::vec3 &e, const glm::vec3 &c, const glm::vec3 &n, float r)
        : Light(e, PI * r * r), m_center(c), m_normal(glm::normalize(n)), m_radius(r), h(1.0f)
    {
        glm::vec3 ez(0.0f, 0.0f, 1.0f), v1(m_normal + ez), v2(m_normal - ez);
        glm::vec3 v = glm::length(v1) > glm::length(v2) ? glm::normalize(v1) : glm::normalize(v2);

        h -= 2.f * glm::mat3(v.x * v.x, v.x * v.y, v.x * v.z,
                             v.y * v.x, v.y * v.y, v.y * v.z,
                             v.z * v.x, v.z * v.y, v.z * v.z);
    }

    virtual glm::vec3 sample() const override
    {
        float r = glm::sqrt(randf()) * m_radius, theta = randf(0.f, 2.f * PI);
        return h * glm::vec3(r * glm::cos(theta), r * glm::sin(theta), 0.f) + m_center;
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
            if (glm::distance(info.pos, m_center) < m_radius)
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