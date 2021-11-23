#ifndef OBJECT_H
#define OBJECT_H
#include "path_tracer/Ray.h"
#include "path_tracer/Math.h"
#include "path_tracer/Material.h"

class Object;

struct IntersectionInfo
{
    bool happen;
    float t;
    glm::vec3 pos;
    glm::vec3 normal;
    Object *obj;
};

class Object
{
public:
    Object(Material *_mat = nullptr) : mat(_mat) {}

    Material *mat;

    virtual float area() const { return 0.f; }
    virtual glm::vec3 sample() const { return glm::vec3(0.f, 0.f, 0.f); }

    virtual ~Object(){};
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) = 0;
};

#endif