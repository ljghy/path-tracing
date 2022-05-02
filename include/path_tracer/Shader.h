#ifndef SHADER_H
#define SHADER_H
#include "path_tracer/Math.h"
#include "path_tracer/Scene.h"
#include "path_tracer/Object.h"
class Shader
{
public:
    virtual ~Shader(){};
    virtual glm::vec3 shade(Scene *scene, Ray ray) = 0;
};
#endif