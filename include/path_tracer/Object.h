#ifndef OBJECT_H
#define OBJECT_H
#include "path_tracer/Ray.h"
#include "path_tracer/Math.h"
#include <cstdint>

class Material;
class Light;

struct IntersectionInfo
{
    bool happen;
    float t;
    glm::vec3 pos;
    glm::vec3 normal;
    Material *mat;
    // bool frontFace = true;
    const Light *light;
};

class Object
{
public:
    Material *m_pMat;
    bool invNorm;

    Object(Material *_mat = nullptr, bool _invNorm = false) : m_pMat(_mat), invNorm(_invNorm) {}
    virtual ~Object(){};

    void attachMat(Material *_mat) { m_pMat = _mat; }
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const = 0;
};

#endif