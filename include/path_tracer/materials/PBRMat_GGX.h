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

    float D_GGX(float NoH, float a) const
    {
        float a2 = sqr(a);
        float f = (NoH * a2 - NoH) * NoH + 1.f;
        return a2 / (PI * f * f);
    }

    glm::vec3 F_Schlick(float u, const glm::vec3 &f0) const
    {
        return f0 + (glm::vec3(1.f) - f0) * glm::pow(1.f - u, 5.f);
    }

    float V_SmithGGXCorrelated(float NoV, float NoL, float a) const
    {
        float a2 = sqr(a);
        float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
        float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
        return 0.5f / (GGXV + GGXL);
    }

    glm::vec3 fr(const glm::vec3 &wi, const glm::vec3 &wo, const glm::vec3 &n) const override
    {
        glm::vec3 h = glm::normalize(wi + wo);
        float NoL = glm::clamp(glm::dot(n, wi), 0.f, 1.f);
        float NoV = glm::abs(glm::dot(n, wo)) + EPSILON;
        float NoH = glm::clamp(glm::dot(n, h), 0.f, 1.f);
        float LoH = glm::clamp(glm::dot(wi, h), 0.f, 1.f);
        float a = sqr(roughness);

        glm::vec3 f0((1.f - metallic) * fresnel + metallic * albedo);
        glm::vec3 F = F_Schlick(LoH, f0);
        float D = D_GGX(NoH, a);
        float V = V_SmithGGXCorrelated(NoV, NoL, a);
        return D * V * F + (glm::vec3(1.f) - F) * (1.f - metallic) * albedo * INV_PI;
    };

    virtual glm::vec3 sample(const glm::vec3 &wi, const glm::vec3 &n, float &cos_inv_pdf) override
    {
        if (metallic < 0.5f)
            return Material::sample(wi, n, cos_inv_pdf);

        float phi = randf(0.f, 2.f * PI), y = randf(0.f, 0.999f), a2(sqr(sqr(roughness)));
        float cos_theta = glm::min(1.f, glm::sqrt((1.f - y) / (1.f + (a2 - 1.f) * y)));
        float sin_theta = glm::sqrt(1.f - cos_theta * cos_theta);

        glm::vec3 v(sin_theta * glm::cos(phi), sin_theta * glm::sin(phi), cos_theta);

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