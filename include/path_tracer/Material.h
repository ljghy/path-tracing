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
    virtual ~Material() {}
};

#endif