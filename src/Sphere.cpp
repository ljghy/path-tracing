#include "path_tracer/objects/Sphere.h"
Sphere::Sphere(const glm::vec3 &_center, float _radius, Material *_mat)
    : Object(_mat), m_center(_center), m_radius(_radius) {}

void Sphere::rayIntersection(const Ray &r, IntersectionInfo &info)
{
    const glm::vec3 &ori = r.origin;
    const glm::vec3 &dir = r.dir;
    glm::vec3 tmp(m_center - ori);
    float p(glm::dot(dir, tmp)), q(tmp.x * tmp.x + tmp.y * tmp.y + tmp.z * tmp.z - m_radius * m_radius);
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
        info.normal = glm::normalize(info.pos - m_center);
        info.obj = this;
        return;
    }
    t = p + delta;
    if (t > 0)
    {
        info.happen = true;
        info.t = t;
        info.pos = ori + t * dir;
        info.normal = glm::normalize(info.pos - m_center);
        info.obj = this;
        return;
    }
    info.happen = false;
}