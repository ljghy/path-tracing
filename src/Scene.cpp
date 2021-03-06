#include "path_tracer/Scene.h"
#include "path_tracer/objects/Objects.h"
#include "path_tracer/materials/Materials.h"
#include "path_tracer/envlights/EnvLights.h"
#include "path_tracer/lights/Lights.h"

#include "utils/JsonParser.hpp"

Scene::Scene()
{
    envLight = [](const glm::vec3 &dir) -> glm::vec3
    { return glm::vec3(1.f, 1.f, 1.f); };
}

Scene::~Scene()
{
    clear();
}

void Scene::clear()
{
    for (auto &pObj : objList)
        delete pObj;
    objList.clear();
    for (auto &pLight : lightList)
        delete pLight;
    lightList.clear();
    for (auto &pMat : matList)
        delete pMat;
    matList.clear();
}

void Scene::rayIntersectionWithScene(const Ray &r, IntersectionInfo &info)
{
    info.t = INFINITY;
    info.happen = false;
    IntersectionInfo objInfo;
    for (auto &obj : objList)
    {
        obj->rayIntersection(r, objInfo);
        if (objInfo.happen && objInfo.t < info.t)
            info = objInfo;
    }

    for (auto &light : lightList)
    {
        light->rayIntersection(r, objInfo);
        if (objInfo.happen && objInfo.t < info.t)
            info = objInfo;
    }
}

static inline glm::vec3 arr2vec3(const JsonNode &j)
{
    return glm::vec3(j[0].getNum(), j[1].getNum(), j[2].getNum());
}

void Scene::loadScene(const std::string &filename)
{
    try
    {
        JsonParser parser;
        parser.loadFromFile(filename.c_str());

        auto err = parser.parse();
        if (err.state != PARSE_SUCCESS)
        {
            err.print();
        }

        auto &data = parser.root();

        if (data.findKey("mat_list"))
        {
            for (auto &p : data["mat_list"].getArr())
            {
                auto &mat = *p;
                if (mat["type"] == "diffuse")
                {
                    matList.push_back(new DiffuseMat(arr2vec3(mat["albedo"])));
                }
                else if (mat["type"] == "pbr_ggx")
                {
                    matList.push_back(new PBRMat_GGX(arr2vec3(mat["fresnel"]),
                                                     arr2vec3(mat["albedo"]),
                                                     mat["roughness"].getNum(),
                                                     mat["metallic"].getNum()));
                }
                else if (mat["type"] == "transparent")
                {
                    matList.push_back(new TransparentMat(mat["refraction"].getNum(),
                                                         arr2vec3(mat["albedo"])));
                }
                else if (mat["type"] == "mirror")
                {
                    matList.push_back(new MirrorMat(arr2vec3(mat["albedo"])));
                }
                else
                    continue;
            }
        }

        if (data.findKey("light_list"))
        {
            for (auto &l : data["light_list"].getArr())
            {
                auto &light = *l;
                if (light["type"] == "disk_light")
                {
                    lightList.push_back(new DiskLight(arr2vec3(light["emission"]),
                                                      arr2vec3(light["center"]),
                                                      arr2vec3(light["normal"]),
                                                      light["radius"].getNum()));
                }
                else if (light["type"] == "rect_light")
                {
                    lightList.push_back(new RectLight(arr2vec3(light["emission"]),
                                                      arr2vec3(light["center"]),
                                                      arr2vec3(light["normal"]),
                                                      arr2vec3(light["x_axis"]),
                                                      light["a"].getNum(),
                                                      light["b"].getNum()));
                }
            }
        }

        if (data.findKey("obj_list"))
        {
            for (auto &p : data["obj_list"].getArr())
            {
                auto &obj = *p;
                if (obj["type"] == "sphere")
                {
                    objList.push_back(new Sphere(arr2vec3(obj["center"]),
                                                 obj["radius"].getNum(),
                                                 matList[obj["mat_index"].toInt()]));
                }
                else if (obj["type"] == "infinite_plane")
                {
                    objList.push_back(new InfinitePlane(arr2vec3(obj["center"]),
                                                        arr2vec3(obj["normal"]),
                                                        matList[obj["mat_index"].toInt()]));
                }
                else if (obj["type"] == "rectangle")
                {
                    objList.push_back(new Rectangle(arr2vec3(obj["center"]),
                                                    arr2vec3(obj["normal"]),
                                                    arr2vec3(obj["x_axis"]),
                                                    obj["a"].getNum(),
                                                    obj["b"].getNum(),
                                                    matList[obj["mat_index"].toInt()]));
                }
                else if (obj["type"] == "box")
                {
                    objList.push_back(new Box(arr2vec3(obj["min"]),
                                              arr2vec3(obj["max"]),
                                              arr2vec3(obj["x_axis"]),
                                              arr2vec3(obj["y_axis"]),
                                              matList[obj["mat_index"].toInt()]));
                }
                else
                    break;
                if (obj.findKey("inv_norm"))
                {
                    objList.back()->invNorm = obj["inv_norm"].getBool();
                }
            }
        }

        if (data.findKey("env_light"))
        {
            auto &env = data["env_light"];
            if (env["type"] == "skybox")
            {
                std::vector<std::string> paths;
                for (int i = 0; i < 6; ++i)
                    paths.push_back(env["path"][i].getStr());
                envLight = SkyboxLight(paths);
            }
            else if (env["type"] == "pure_color")
            {
                envLight = PureColorEnvLight(arr2vec3(env["color"]));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        throw std::runtime_error("Failed to load scene from \"" + filename + "\".");
    }
}
