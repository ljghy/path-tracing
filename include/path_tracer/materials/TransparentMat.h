#ifndef TRANSPARENT_MAT_H
#define TRANSPARENT_MAT_H

#include "path_tracer/Material.h"

class TransparentMat : public Material
{

public:
    glm::vec3 albedo;
    float ir;

    virtual bool isStdBRDF() override { return false; }

    TransparentMat(float _ir = 1.3f, const glm::vec3 &a = glm::vec3(1.0f, 1.0f, 1.0f)) : ir(_ir), albedo(a) {}
    glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const
    {
        return albedo * INV_PI;
    }

    static float reflectance(float cosine, float _ir)
    {
        auto r0 = (1.f - _ir) / (1.f + _ir);
        r0 = r0 * r0;
        return r0 + (1.f - r0) * pow((1.f - cosine), 5.f);
    }

    glm::vec3 sample(const glm::vec3 &wi, const glm::vec3 &n, float &cos_inv_pdf) override
    {
        cos_inv_pdf = PI;
        float ci = CLAMP(glm::dot(wi, n), -1.f, 1.f);
        float eta = ci > 0 ? (1.f / ir) : ir;
        float co = 1.0f - (1.0f - ci * ci) * eta * eta;

        if (co < 0.f || randf() < reflectance(fabs(ci), eta))
        {
            return 2.0f * ci * n - wi;
        }

        co = glm::sqrt(co);
        if (ci > 0) // front face
        {
            return (-co + ci * eta) * n - eta * wi;
        }
        else
        {
            return (co + ci * eta) * n - eta * wi;
        }
    }
};

#endif
