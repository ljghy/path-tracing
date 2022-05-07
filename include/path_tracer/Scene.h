#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <functional>
#include <string>

#include "path_tracer/Object.h"
#include "path_tracer/Material.h"
#include "Light.h"

struct Scene
{
    std::vector<Object *> objList;
    std::vector<Light *> lightList;
    std::vector<Material *> matList;

    Scene();
    ~Scene();
    std::function<glm::vec3(const glm::vec3 &)> envLight;
    void rayIntersectionWithScene(const Ray &r, IntersectionInfo &info);

    void loadScene(const std::string &);
    void clear();
};
#endif