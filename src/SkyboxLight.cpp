#include "path_tracer/envlights/SkyboxLight.h"
#include <cstdint>
static struct
{
    glm::vec3 n0;
    uint8_t code;
    uint8_t mask;
    glm::vec3 n;
    glm::vec3 u;
    glm::vec3 v;
} FaceInfo[6]{
    {glm::vec3(0, 1, 1), 40, 60, PX, PY, NZ},  // x+ 1010?? 111100
    {glm::vec3(0, -1, 1), 20, 60, NX, NY, NZ}, // x- 0101?? 111100
    {glm::vec3(-1, 0, 1), 49, 51, PY, NX, NZ}, // y+ 11??01 110011
    {glm::vec3(1, 0, 1), 2, 51, NY, PX, NZ},   // y- 00??10 110011
    {glm::vec3(-1, 1, 0), 15, 15, PZ, PY, PX}, // z+ ??1111 001111
    {glm::vec3(1, 1, 0), 0, 15, NZ, PY, NX}    // z- ??0000 001111
};

SkyboxLight::SkyboxLight(const std::vector<std::string> &paths)
{
    assert(paths.size() == 6);
    for (int i = 0; i < 6; ++i)
        faces[i] = std::make_shared<Bitmap>(paths[i], true);
}

SkyboxLight::~SkyboxLight() {}

glm::vec3 SkyboxLight::operator()(const glm::vec3 &dir) const
{
    uint8_t byteFlag = 0;
    for (uint8_t i = 0; i < 6; ++i)
        byteFlag |= (glm::dot(dir, FaceInfo[i].n0) > 0) << i;
    for (int i = 0; i < 6; ++i)
    {
        uint8_t t = (byteFlag ^ FaceInfo[i].code) & FaceInfo[i].mask;
        if (!t)
        {
            glm::vec3 p = dir / glm::dot(FaceInfo[i].n, dir);
            float ut = 0.5f * (glm::dot(FaceInfo[i].u, p) + 1.f),
                  vt = 0.5f * (glm::dot(FaceInfo[i].v, p) + 1.f);
            return faces[i]->sample(ut, vt);
        }
    }
    return ERR_COLOR;
}