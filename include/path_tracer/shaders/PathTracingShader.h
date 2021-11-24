#ifndef PATH_TRACING_SHADER
#define PATH_TRACING_SHADER

#include "path_tracer/Shader.h"

class PathTracingShader : public Shader
{
protected:
    int maxDepth;

public:
    PathTracingShader(int n = 3) : maxDepth(n) {}
    virtual glm::vec3 shade(Scene *scene, const Ray &ray, int depth) override;
};
#endif