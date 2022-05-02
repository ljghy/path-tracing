#ifndef LIGHT_H
#define LIGHT_H

#include "path_tracer/Math.h"
#include "path_tracer/Object.h"

class Light
{
protected:
    unsigned int id;

public:
    static uint32_t lightCount;

    glm::vec3 emission;
    float area;

    Light(const glm::vec3 &e, float a) : emission(e), area(a), id(lightCount)
    {
        ++lightCount;
    }

    unsigned int getId() const { return id; }

    virtual glm::vec3 sample() const = 0;
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const = 0;
};

inline uint32_t Light::lightCount = 0;

#endif