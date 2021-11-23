#ifndef ENV_LIGHT_H
#define ENV_LIGHT_H

#include "path_tracer/Math.h"

class EnvLight
{
public:
    virtual ~EnvLight() {}
    virtual glm::vec3 operator()(const glm::vec3 &dir) const = 0;
};

#endif