#ifndef MIRROR_MAT_H
#define MIRROR_MAT_H

#include "path_tracer/Material.h"

class MirrorMat : public Material
{

public:
    glm::vec3 albedo;

    virtual bool isStdBRDF() override { return false; }

    MirrorMat(const glm::vec3 &a = glm::vec3(1.0f, 1.0f, 1.0f)) : albedo(a) {}
    glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const
    {
        return albedo * INV_PI;
    }

    glm::vec3 sample(const glm::vec3 &wi, const glm::vec3 &n, float &cos_inv_pdf) override
    {
        cos_inv_pdf = PI;
        return 2.0f * glm::dot(wi, n) * n - wi;
    }
};

#endif
