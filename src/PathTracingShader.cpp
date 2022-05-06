#include "path_tracer/shaders/PathTracingShader.h"
#include <vector>
glm::vec3 PathTracingShader::shade(Scene *scene, Ray ray)
{
    IntersectionInfo info;
    std::shared_ptr<Material> preMat;
    glm::vec3 result(0.f, 0.f, 0.f);

    std::vector<glm::vec3> dirLightStack, indirLightStack;
    dirLightStack.reserve(maxDepth);
    indirLightStack.reserve(maxDepth);

    for (unsigned int d = 0; d < maxDepth; ++d)
    {
        preMat = info.mat;
        scene->rayIntersectionWithScene(ray, info);
        if (info.happen)
        {
            glm::vec3 dirLight(0.f, 0.f, 0.f);
            if (info.mat != nullptr && !info.mat->emissive && info.mat->isStdBRDF())
            {
                for (auto &light : scene->lightList)
                {
                    glm::vec3 wi = light->sample() - info.pos;
                    float d = glm::length(wi);
                    if (ISZERO(d))
                        return glm::vec3(0.f);
                    wi /= d;

                    Ray r(info.pos + EPSILON * wi, wi);
                    IntersectionInfo lightInfo;
                    scene->rayIntersectionWithScene(r, lightInfo);
                    if (lightInfo.happen && lightInfo.mat == nullptr && lightInfo.id == light->getId())
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
                if (!info.mat->emissive)
                {
                    glm::vec3 wi = info.mat->sample(-ray.dir, info.normal, cos_inv_pdf);
                    indirLightStack.push_back(info.mat->fr(wi, -ray.dir, info.normal) * cos_inv_pdf);
                    ray = Ray(info.pos + EPSILON * wi, wi);
                }
                else
                {
                    if (d == 0)
                        return info.mat->emission;
                    dirLightStack.pop_back();
                    result = info.mat->emission;
                    break;
                }
            }
            else // hit light
            {
                if (d > 0 && preMat->isStdBRDF())
                {
                    dirLightStack.pop_back();
                    break;
                }

                for (auto &light : scene->lightList)
                {
                    if (light->getId() == info.id)
                    {
                        result = light->emission * glm::max(0.f, glm::dot(info.normal, -ray.dir));
                        break;
                    }
                }
                if (d == 0)
                    return result;

                dirLightStack.pop_back();
                break;
            }
        }
        else
        {
            result = scene->envLight(ray.dir);
            break;
        }
    }
    while (!dirLightStack.empty())
    {
        result = dirLightStack.back() + result * indirLightStack.back();
        dirLightStack.pop_back();
        indirLightStack.pop_back();
    }
    return result;
}