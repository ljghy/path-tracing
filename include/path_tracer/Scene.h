#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <functional>
#include <string>
#include <memory>

#include "path_tracer/Object.h"
#include "path_tracer/Material.h"
#include "Light.h"

struct Scene
{
    Scene();
    std::vector<std::shared_ptr<Object>> objList;
    std::vector<std::shared_ptr<Light>> lightList;
    std::vector<std::shared_ptr<Material>> matList;

    std::function<glm::vec3(const glm::vec3 &)> envLight;
    void rayIntersectionWithScene(const Ray &r, IntersectionInfo &info);

    void loadScene(const std::string &);
    void clear();
};
#endif