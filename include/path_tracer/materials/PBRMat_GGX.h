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
        glm::vec3 one(1.f, 1.f, 1.f);
        glm::vec3 f0((1.f - metallic) * fresnel + metallic * albedo);
        glm::vec3 h = glm::normalize(wi + wo);
        glm::vec3 f = f0 + (one - f0) * powf(1.f - glm::dot(wi, h), 5.f);
        float a2(sqr(sqr(roughness)));
        float d = a2 * INV_PI / (sqr(1.f + sqr(glm::dot(n, h)) * (a2 - 1.f)));
        float k = sqr(roughness + 1.f) * 0.125f;
        float g = 1.f / ((glm::dot(n, wi) * (1.f - k) + k) * (glm::dot(n, wo) * (1.f - k) + k));

        return f * g * d * 0.25f + (one - f) * (1.f - metallic) * albedo * INV_PI;
    };
/*
    virtual glm::vec3 sample(const glm::vec3 &wi, const glm::vec3 &n, float &cos_inv_pdf) override
    {
        if (metallic * (1.f - roughness) < 0.5f)
        {
            float cos_theta = sqrt(1.f - randf()), phi = 2 * PI * randf(), sin_theta = sqrt(1.f - cos_theta * cos_theta);
            cos_inv_pdf = PI;
            glm::vec3 v(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
            float t = n.z - 1.f;
            if (ISZERO(t))
                return v;
            t = 1.f / t;
            return glm::vec3((1.f + n.x * n.x * t) * v.x + n.x * n.y * t * v.y + n.x * v.z,
                             n.x * n.y * t * v.x + (1.f + n.y * n.y * t) * v.y + n.y * v.z,
                             n.x * v.x + n.y * v.y + n.z * v.z);
        }
        float a2 = sqr(sqr(roughness));
        float X1 = randf(), phi = 2 * PI * randf();

        float cos_theta = sqrt((1.f - X1) / (1.f + (a2 - 1.f) * X1));
        float sin_theta = sqrt(1.f - sqr(cos_theta));

        glm::vec3 v(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
        glm::vec3 h;
        float t = n.z - 1.f;
        if (ISZERO(t))
            h = v;
        else
        {
            t = 1.f / t;
            h = glm::vec3((1.f + n.x * n.x * t) * v.x + n.x * n.y * t * v.y + n.x * v.z,
                          n.x * n.y * t * v.x + (1.f + n.y * n.y * t) * v.y + n.y * v.z,
                          n.x * v.x + n.y * v.y + n.z * v.z);
        }
        auto ret = 2.f * glm::dot(wi, h) * h - wi;
        if (glm::dot(n, ret) < 0.f)
            cos_inv_pdf = 0.f;
        else
            cos_inv_pdf = 4.f * PI * sqr((cos_theta * a2 - cos_theta) * cos_theta + 1.f) / a2 * glm::dot(ret, n);
        return ret;
    }*/
};

#endif