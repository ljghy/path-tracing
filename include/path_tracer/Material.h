#ifndef MATERIAL_H
#define MATERIAL_H

#include "path_tracer/Math.h"
class Material
{
public:
    bool emissive;
    glm::vec3 emission;

    Material(bool _emissive = false, const glm::vec3 &_emission = glm::vec3(0.f, 0.f, 0.f))
        : emissive(_emissive), emission(_emission) {}

    virtual glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const = 0;
    virtual glm::vec3 sampleInHemisphere(const glm::vec3 & wi, const glm::vec3 &n, float &cos_inv_pdf)
    {
        float cos_theta = randf(), phi = 2 * PI * randf(), sin_theta = sqrt(1.f - cos_theta * cos_theta);
        cos_inv_pdf = cos_theta * 2.f * PI;
        glm::vec3 v(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
        float t = n.z - 1.f;
        if (ISZERO(t))
            return v;
        t = 1.f / t;
        return glm::vec3((1.f + n.x * n.x * t) * v.x + n.x * n.y * t * v.y + n.x * v.z,
                         n.x * n.y * t * v.x + (1.f + n.y * n.y * t) * v.y + n.y * v.z,
                         n.x * v.x + n.y * v.y + n.z * v.z);
    }
    virtual ~Material() {}
};

#endif