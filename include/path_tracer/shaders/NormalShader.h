#ifndef NORMAL_SHADER_H
#define NORMAL_SHADER_H

#include "path_tracer/Shader.h"
class NormalShader : public Shader
{
public:
    virtual glm::vec3 shade(Scene *scene, Ray ray) override
    {
        IntersectionInfo info;
        scene->rayIntersectionWithScene(ray, info);
        if (info.happen)
            return info.normal;
        else
            return ERR_COLOR;
    }
};

#endif