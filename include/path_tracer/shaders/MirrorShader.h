#ifndef MIRROR_SHADER_H
#define MIRROR_SHADER_H

#include "path_tracer/Shader.h"

class MirrorShader : public Shader
{
public:
    int maxDepth;
    MirrorShader(int n = 3) : maxDepth(n) {}

    virtual glm::vec3 shade(Scene *scene, const Ray &ray, int depth) override
    {
        if (depth >= maxDepth)
            return glm::vec3(0.f, 0.f, 0.f);
        IntersectionInfo info;
        scene->rayIntersectionWithScene(ray, info);

        if (info.happen)
        {
            if (info.obj->mat != nullptr)
            {
                if (info.obj->mat->emissive)
                    return info.obj->mat->emission;
                else
                    return shade(scene, Ray(info.pos + info.normal * EPSILON, ray.dir - 2.f * info.normal * glm::dot(info.normal, ray.dir)), depth + 1);
            }
            else
                return ERR_COLOR;
        }
        else
            return scene->envLight(ray.dir);
    }
};
#endif