#include "path_tracer/shaders/PathTracingShader.h"
glm::vec3 PathTracingShader::sampleHemisphere(const glm::vec3 &n)
{
    float cos_theta = randf(), phi = 2 * PI * randf(), sin_theta = sqrt(1.f - cos_theta * cos_theta);
    glm::vec3 v(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
    float t = n.z - 1.f;
    if (ISZERO(t))
        return v;
    t = 1.f / t;
    return glm::vec3((1.f + n.x * n.x * t) * v.x + n.x * n.y * t * v.y + n.x * v.z,
                     n.x * n.y * t * v.x + (1.f + n.y * n.y * t) * v.y + n.y * v.z,
                     n.x * v.x + n.y * v.y + n.z * v.z);
}

glm::vec3 PathTracingShader::shade(Scene *scene, const Ray &ray, int depth)
{
    if (depth >= maxDepth)
        return glm::vec3(0.f, 0.f, 0.f);
    IntersectionInfo info;
    scene->rayIntersectionWithScene(ray, info);

    glm::vec3 indirLight(0.f, 0.f, 0.f);

    if (info.happen)
    {
        if (info.obj->mat != nullptr)
        {
            if (!info.obj->mat->emissive)
            {
                glm::vec3 wi = sampleHemisphere(info.normal);
                Ray r(info.pos + info.normal * EPSILON, wi);
                indirLight = shade(scene, r, depth + 1) * info.obj->mat->fr(wi, -ray.dir, info.normal) * glm::dot(info.normal, wi) * 2.f * PI;
            }
        }
    }
    else
        indirLight = scene->envLight(ray.dir);
    return indirLight;
}