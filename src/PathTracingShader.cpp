#include "path_tracer/shaders/PathTracingShader.h"

glm::vec3 PathTracingShader::shade(Scene *scene, const Ray &ray, int depth)
{
    if (depth >= maxDepth)
        return glm::vec3(0.f, 0.f, 0.f);
    IntersectionInfo info;
    scene->rayIntersectionWithScene(ray, info);

    glm::vec3 indirLight(0.f, 0.f, 0.f);

    if (info.happen)
    {
        auto &m = info.obj->mat;
        if (m != nullptr)
        {
            if (!m->emissive)
            {
                float cos_inv_pdf;
                glm::vec3 wi = m->sampleInHemisphere(-ray.dir, info.normal, cos_inv_pdf);
                Ray r(info.pos + info.normal * EPSILON, wi);
                indirLight = shade(scene, r, depth + 1) * m->fr(wi, -ray.dir, info.normal) * cos_inv_pdf;
            }
        }
    }
    else
        indirLight = scene->envLight(ray.dir);
    return indirLight;
}