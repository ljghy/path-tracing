#ifndef SKYBOX_LIGHT_H
#define SKYBOX_LIGHT_H

#include "path_tracer/EnvLight.h"
#include "path_tracer/Bitmap.h"
#include <vector>
#include <string>
#include <memory>

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