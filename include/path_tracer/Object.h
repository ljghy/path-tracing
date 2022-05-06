#ifndef OBJECT_H
#define OBJECT_H
#include "path_tracer/Ray.h"
#include "path_tracer/Math.h"
#include "path_tracer/Material.h"
#include <memory>
#include <cstdint>

struct IntersectionInfo
{
    bool happen;
    float t;
    glm::vec3 pos;
    glm::vec3 normal;
    std::shared_ptr<Material> mat;
    // bool frontFace = true;
    uint32_t id;
};

class Object
{
public:
    std::shared_ptr<Material> m_pMat;
    bool invNorm;

    Object(const std::shared_ptr<Material> _mat = nullptr, bool _invNorm = false) : m_pMat(_mat), invNorm(_invNorm) {}
    virtual ~Object(){};

    void attachMat(std::shared_ptr<Material> _mat) { m_pMat = _mat; }
    virtual void rayIntersection(const Ray &r, IntersectionInfo &info) const = 0;
};

#endif