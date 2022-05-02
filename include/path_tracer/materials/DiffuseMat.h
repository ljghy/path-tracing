#ifndef DIFFUSE_MAT_H
#define DIFFUSE_MAT_H

#include "path_tracer/Material.h"

class DiffuseMat : public Material
{
public:
    glm::vec3 albedo;

    DiffuseMat(const glm::vec3 &a) : albedo(a) {}
    glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const
    {
        return albedo * INV_PI;
    }
};
#endif