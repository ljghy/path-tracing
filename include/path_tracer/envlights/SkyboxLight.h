#ifndef SKYBOX_LIGHT_H
#define SKYBOX_LIGHT_H

#include "path_tracer/EnvLight.h"
#include "path_tracer/Bitmap.h"
#include <vector>
#include <string>
#include <memory>

#define PX glm::vec3(1.f, 0.f, 0.f)
#define NX glm::vec3(-1.f, 0.f, 0.f)
#define PY glm::vec3(0.f, 1.f, 0.f)
#define NY glm::vec3(0.f, -1.f, 0.f)
#define PZ glm::vec3(0.f, 0.f, 1.f)
#define NZ glm::vec3(0.f, 0.f, -1.f)

class SkyboxLight : public EnvLight
{
protected:
    std::shared_ptr<Bitmap> faces[6]; // px, nx, py, ny, pz, nz

public:
    SkyboxLight(const std::vector<std::string> &);
    virtual ~SkyboxLight();

    virtual glm::vec3 operator()(const glm::vec3 &) const override;
};

#endif