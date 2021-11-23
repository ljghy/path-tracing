#ifndef PBR_MAT_H
#define PBR_MAT_H

#include "path_tracer/Material.h"

class PBRMat : public Material
{
protected:
    glm::vec3 fresnel;
    glm::vec3 albedo;
    float roughness;
    float metallic;

public:
    PBRMat(const glm::vec3 &f, const glm::vec3 &a, float r, float m)
        : fresnel(f), albedo(a), roughness(r), metallic(m) {}

    glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const
    {
        glm::vec3 one(1.f, 1.f, 1.f);
        glm::vec3 f0(metallic * fresnel + (1.f - metallic) * albedo);
        glm::vec3 f = f0 + (one - f0) * powf(1.f - glm::dot(wi, n), 5.f);
        glm::vec3 h = glm::normalize(wi + wo);
        float a(sqr(sqr(roughness)));
        float d = a / (PI * sqr(1.f + sqr(glm::dot(n, h)) * (a - 1.f)));
        float k = sqr(roughness + 1.f) * 0.125f;
        float g = 1.f / ((glm::dot(n, wi) * (1.f - k) + k) * (glm::dot(n, wo) * (1.f - k) + k));
        return f * g * d * 0.25f + (one - f) * (1.f - metallic) * albedo / PI;
    };
};

#endif