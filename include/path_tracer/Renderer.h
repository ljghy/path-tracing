#ifndef RENDERER_H
#define RENDERER_H
#include "path_tracer/Scene.h"
#include "path_tracer/Bitmap.h"
#include "path_tracer/Camera.h"
#include "path_tracer/Shader.h"
#include <cstdint>
#include <string>

class Renderer
{
private:
    Camera *camera;
    Shader *shader;
    uint16_t SSP;
    Bitmap *renderResult;

    std::string outputPath;
    float scale;
    bool gamma;

    bool fromFile;

    void free();
public:
    Renderer()
        : camera(nullptr),
          shader(nullptr),
          SSP(0),
          renderResult(nullptr),
          outputPath(""),
          scale(1.f),
          gamma(false),
          fromFile(false) {}
    Renderer(uint16_t w, uint16_t h, uint16_t _SSP, Camera *, Shader *, const std::string &, float = 1.f, bool = false);
    ~Renderer();
    friend void renderThread(Renderer *renderer, Scene *scene, uint8_t id, uint16_t threadNum);
    void render(Scene *scene);
    bool loadConfig(const std::string &);
    void outputPNG();
};
#endif