#include "path_tracer/shaders/PathTracingShader.h"
#include <vector>
glm::vec3 PathTracingShader::shade(Scene *scene, Ray ray)
{
    IntersectionInfo info{};
    Material *preMat;
    glm::vec3 result(0.f, 0.f, 0.f);

    std::vector<glm::vec3> dirLightStack, indirLightStack;
    dirLightStack.reserve(maxDepth);
    indirLightStack.reserve(maxDepth);

    for (unsigned int d = 0; d < maxDepth; ++d)
    {
        preMat = info.mat;
        scene->rayIntersectionWithScene(ray, info);
        if (!info.happen)
        {
            result = scene->envLight(ray.dir);
            break;
        }

        glm::vec3 dirLight(0.f, 0.f, 0.f);
        if (info.mat != nullptr && info.mat->isStdBRDF())
        {
            for (auto &light : scene->lightList)
            {
                glm::vec3 wi = light->sample() - info.pos;
                float d = glm::length(wi);
                if (ISZERO(d))
                    continue;
                wi /= d;

                Ray r(info.pos + EPSILON * wi, wi);
                IntersectionInfo lightInfo{};
                scene->rayIntersectionWithScene(r, lightInfo);
                if (lightInfo.happen && lightInfo.light == light)
                {
                    dirLight += light->emission * info.mat->fr(wi, -ray.dir, info.normal) *
                                glm::abs(glm::dot(info.normal, wi)) *
                                glm::max(0.f, glm::dot(lightInfo.normal, -wi)) * light->area / (d * d);
                }
            }
        }
        dirLightStack.push_back(dirLight);

        if (info.mat != nullptr)
        {
            float cos_inv_pdf;
            glm::vec3 wi = info.mat->sample(-ray.dir, info.normal, cos_inv_pdf);
            indirLightStack.push_back(info.mat->fr(wi, -ray.dir, info.normal) * cos_inv_pdf);
            ray = Ray(info.pos + EPSILON * wi, wi);
        }
        else // hit light
        {
            dirLightStack.pop_back();
            if (d > 0 && preMat->isStdBRDF())
                break;

            result = info.light->emission * glm::max(0.f, glm::dot(info.normal, -ray.dir));
            if (d == 0)
                return result;

            break;
        }
    }
    for (size_t i = dirLightStack.size(); i > 0; --i)
        result = dirLightStack[i - 1] + result * indirLightStack[i - 1];
    return result;
}