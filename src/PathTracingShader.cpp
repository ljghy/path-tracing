#include "path_tracer/shaders/PathTracingShader.h"
#include <vector>
glm::vec3 PathTracingShader::shade(Scene *scene, Ray ray)
{
    IntersectionInfo info, preInfo;
    Ray preRay;
    glm::vec3 result(0.f, 0.f, 0.f);

    std::vector<glm::vec3> dirLightStack, indirLightStack;
    dirLightStack.reserve(maxDepth);
    indirLightStack.reserve(maxDepth);

    for (unsigned int d = 0; d < maxDepth; ++d)
    {
        preInfo = info;
        preRay = ray;

        scene->rayIntersectionWithScene(ray, info);
        if (info.happen)
        {
            glm::vec3 dirLight(0.f, 0.f, 0.f);
            if (info.mat != nullptr && info.mat->isStdBRDF())
            {
                for (auto &light : scene->lightList)
                {
                    glm::vec3 wi = light->sample() - info.pos;
                    float d = glm::length(wi);
                    wi /= d;

                    Ray r(info.pos + EPSILON * wi, wi);
                    IntersectionInfo lightInfo;
                    scene->rayIntersectionWithScene(r, lightInfo);
                    if (lightInfo.happen && lightInfo.mat == nullptr && lightInfo.id == light->getId())
                    {
                        dirLight += light->emission * info.mat->fr(wi, -ray.dir, info.normal) *
                                    std::fabs(glm::dot(info.normal, wi)) *
                                    std::max(0.f, glm::dot(lightInfo.normal, -wi)) * light->area / (d * d);
                    }
                }
            }
            dirLightStack.push_back(dirLight);

            if (info.mat != nullptr)
            {
                float cos_inv_pdf;
                glm::vec3 wi = info.mat->sample(-ray.dir, info.normal, cos_inv_pdf);

                if (!info.mat->emissive)
                {
                    indirLightStack.push_back(info.mat->fr(wi, -ray.dir, info.normal) * cos_inv_pdf);
                    ray = Ray(info.pos + EPSILON * wi, wi);
                }
                else
                {
                    result = info.mat->emission * info.mat->fr(wi, -ray.dir, info.normal) * cos_inv_pdf;
                    break;
                }
            }
            else // hit light
            {
                if (d > 0 && preInfo.mat->isStdBRDF())
                    break;
                for (auto &light : scene->lightList)
                {
                    if (light->getId() == info.id)
                    {
                        result = light->emission * std::max(0.f, glm::dot(info.normal, -ray.dir));
                        break;
                    }
                }
                if (d == 0)
                    break;

                float cos_inv_pdf;
                glm::vec3 wi = preInfo.mat->sample(-preRay.dir, preInfo.normal, cos_inv_pdf);
                result = result * preInfo.mat->fr(wi, -preRay.dir, preInfo.normal) * cos_inv_pdf;
                break;
            }
        }
        else
        {
            result = scene->envLight(ray.dir);
            break;
        }
    }
    while (!indirLightStack.empty())
    {
        result = dirLightStack.back() + result * indirLightStack.back();
        dirLightStack.pop_back();
        indirLightStack.pop_back();
    }
    return result;
}