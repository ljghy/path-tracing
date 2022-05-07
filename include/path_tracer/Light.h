#ifndef LIGHT_H
#define LIGHT_H

#include "path_tracer/Math.h"
#include "path_tracer/Object.h"

class Light
{
public:
    glm::vec3 emission;
    float area;

    Light(const glm::vec3 &e, float a) : emission(e), area(a) {}
    virtual ~Light() = default;

    virtual glm::vec3 sample() const = 0;
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const = 0;
};

#endif