#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "path_tracer/Object.h"

class Rectangle : public Object
{
protected:
    glm::vec3 m_center;
    glm::vec3 m_normal;
    glm::vec3 m_xAxis;
    glm::vec3 m_yAxis;
    float m_a, m_ha;
    float m_b, m_hb;

public:
    Rectangle(const glm::vec3 &c, const glm::vec3 &n, const glm::vec3 &x, float a, float b,
              const std::shared_ptr<Material> &_mat = nullptr, bool _invNorm = false);
    void rayIntersection(const Ray &r, IntersectionInfo &info) const;
};

#endif