#ifndef PATH_TRACING_SHADER
#define PATH_TRACING_SHADER

#include "path_tracer/Shader.h"

class PathTracingShader : public Shader
{
protected:
    unsigned int maxDepth;

public:
    PathTracingShader(unsigned int d = 6) : maxDepth(d) {}
    virtual glm::vec3 shade(Scene *scene, Ray ray) override;
};
#endif