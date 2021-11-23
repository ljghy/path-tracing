#ifndef RAY_H
#define RAY_H

#include "path_tracer/Math.h"
struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir;
    Ray(const glm::vec3 &o = glm::vec3(0.f, 0.f, 0.f), const glm::vec3 &d = glm::vec3(1.f, 0.f, 0.f))
        : origin(o), dir(glm::normalize(d)) {}
};

#endif