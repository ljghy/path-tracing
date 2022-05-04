#ifndef PBR_MAT_H
#define PBR_MAT_H

#include "path_tracer/Material.h"
#include <stdio.h>

class PBRMat_GGX : public Material
{
protected:
    glm::vec3 fresnel;
    glm::vec3 albedo;
    float roughness;
    float metallic;

public:
    PBRMat_GGX(const glm::vec3 &f, const glm::vec3 &a, float r, float m)
        : fresnel(f), albedo(a), roughness(r), metallic(m) {}

    glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const
    {
        glm::vec3 h = wi + wo;
        if (glm::dot(n, h) <= 0.f || ISZERO(glm::length(h)))
            return glm::vec3(0.f);
        h = glm::normalize(h);
        glm::vec3 one(1.f, 1.f, 1.f);
        glm::vec3 f0((1.f - metallic) * fresnel + metallic * albedo);
        glm::vec3 f = f0 + (one - f0) * powf(1.f - std::min(glm::dot(wo, h), 1.f), 5.f);
        float a(sqr(roughness)), a2(sqr(a));

        float noh2 = std::min(sqr(glm::dot(n, h)), 1.f);
        float d = INV_PI / sqr((1.f - noh2) / a + noh2 * a);

        float k = sqr(roughness + 1.f) * 0.125f;
        float g = 1.f / ((glm::dot(n, wi) * (1.f - k) + k) * (glm::dot(n, wo) * (1.f - k) + k));
        return f * g * d * 0.25f + (one - f) * (1.f - metallic) * albedo * INV_PI;
    };

    virtual glm::vec3 sample(const glm::vec3 &wi, const glm::vec3 &n, float &cos_inv_pdf) override
    {
        if (metallic < 0.5f)
            return Material::sample(wi, n, cos_inv_pdf);

        float phi = randf(0.f, 2.f * PI), y = randf(0.f, 0.999f), a2(sqr(sqr(roughness)));
        float cos_theta = std::min(1.f, sqrt((1.f - y) / (1.f + (a2 - 1.f) * y)));
        float sin_theta = sqrt(1.f - cos_theta * cos_theta);

        glm::vec3 v(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);

        glm::vec3 ez(0.0f, 0.0f, 1.0f), w(n - ez);
        if (!ISZERO(glm::length(w)))
        {
            w = glm::normalize(w);
            glm::mat3 h(1.f);
            h -= 2.f * glm::mat3(w.x * w.x, w.x * w.y, w.x * w.z,
                                 w.y * w.x, w.y * w.y, w.y * w.z,
                                 w.z * w.x, w.z * w.y, w.z * w.z);
            v = h * v;
        }
        glm::vec3 wo = 2.f * glm::dot(wi, v) * v - wi;

        float d = (a2 - 1.f) * cos_theta * cos_theta + 1.f;
        cos_inv_pdf = d / (a2 * cos_theta) * PI * d * std::max(0.f, glm::dot(n, wo));
        return wo;
    }
};

#endif