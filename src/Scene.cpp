#include "path_tracer/Scene.h"
#include "path_tracer/objects/Objects.h"
#include "path_tracer/materials/Materials.h"
#include "path_tracer/envlights/EnvLights.h"
#include "utils/JsonParser.hpp"

Scene::Scene()
    : fromFile(false)
{
    envLight = [](const glm::vec3 &dir) -> glm::vec3
    { return glm::vec3(0.f, 0.f, 0.f); };
}

void Scene::init()
{
    objList.clear();
    lightList.clear();
    matList.clear();
}

void Scene::free()
{
    if (fromFile)
    {
        for (auto &p : objList)
            delete p;
        objList.clear();
        for (auto &p : lightList)
            delete p;
        lightList.clear();
        for (auto &p : matList)
            delete p;
    }
    init();
}
Scene::~Scene()
{
    free();
}

void Scene::rayIntersectionWithScene(const Ray &r, IntersectionInfo &info)
{
    info.t = INFINITY;
    info.happen = false;
    IntersectionInfo objInfo;
    for (auto &obj : objList)
    {
        obj->rayIntersection(r, objInfo);
        if (objInfo.happen)
        {
            info.happen = true;
            if (objInfo.t < info.t)
            {
                info.t = objInfo.t;
                info.pos = objInfo.pos;
                info.normal = objInfo.normal;
                info.obj = obj;
            }
        }
    }

    for (auto &obj : lightList)
    {
        obj->rayIntersection(r, objInfo);
        if (objInfo.happen)
        {
            info.happen = true;
            if (objInfo.t < info.t)
            {
                info.t = objInfo.t;
                info.pos = objInfo.pos;
                info.normal = objInfo.normal;
                info.obj = obj;
            }
        }
    }
}

static inline glm::vec3 arr2vec3(const JsonNode &j)
{
    return glm::vec3(j[0].num, j[1].num, j[2].num);
}

bool Scene::loadScene(const std::string &filename)
{
    JsonParser parser;
    parser.loadFromFile(filename.c_str());

    auto err = parser.parse();
    if (err.state != PARSE_SUCCESS)
    {
        JsonParser::PrintErrMsg(err);
        return false;
    }

    try
    {
        free();
        fromFile = true;
        auto &data = *(parser.root);

        if (data.findKey("mat_list"))
        {
            for (auto &p : data["mat_list"].arr)
            {
                auto &mat = *p;
                if (mat["type"].str == "diffuse")
                {
                    matList.push_back(new DiffuseMat(arr2vec3(mat["albedo"])));
                }
                else if (mat["type"].str == "pbr")
                {
                    matList.push_back(new PBRMat(arr2vec3(mat["fresnel"]),
                                                 arr2vec3(mat["albedo"]),
                                                 mat["roughness"].num,
                                                 mat["metallic"].num));
                }
            }
        }

        if (data.findKey("obj_list"))
        {
            for (auto &p : data["obj_list"].arr)
            {
                auto &obj = *p;
                if (obj["type"].str == "sphere")
                {
                    objList.push_back(new Sphere(arr2vec3(obj["center"]),
                                                 obj["radius"].num,
                                                 matList[obj["mat_index"].toInt()]));
                }
            }
        }

        if (data.findKey("env_light"))
        {
            auto &env = data["env_light"];
            if (env["type"].str == "skybox")
            {
                std::vector<std::string> paths;
                for (int i = 0; i < 6; ++i)
                    paths.push_back(env["path"][i].str);
                envLight = SkyboxLight(paths);
            }
        }
    }
    catch (const std::exception &e)
    {
        printf("%s\n", e.what());
        return false;
    }

    return true;
}
